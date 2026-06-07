
#include "client_game_loop.h"

#include <iostream>
#include <utility>

#include <SDL.h>

#include "config/cargador_nombres_item.h"
#include "audio/gestor_audio.h"

#ifndef CLIENT_GAME_CONFIG_PATH
#define CLIENT_GAME_CONFIG_PATH "config/game_config.toml"
#endif

#ifndef CLIENT_ASSETS_DIR
#define CLIENT_ASSETS_DIR "client/interface"
#endif

// constexpr int MAX_FRAMESKIP = 5;


ClientGameLoop::ClientGameLoop(Queue<MensajeServidor>& server_messages,
                               ClientBusiness& business,
                               const uint16_t idCliente,
                               const ConfigCliente& config):
    server_messages(server_messages),
    business(business),
    object_state(idCliente),
    config(config),
    parser(CargadorNombresItem().cargar(CLIENT_GAME_CONFIG_PATH)),
    is_running(false) {}

ClientGameLoop::~ClientGameLoop() = default;

void ClientGameLoop::init(const char* title,
                          const int xpos,
                          const int ypos,
                          const int width,
                          const int height,
                          const bool fullscreen) {
    object_renderer.init(title, xpos, ypos, width, height, fullscreen, config.vsync,
                         config.fpsMax);
    handler.set_window_dimensions(width, height);
    handler.setIdCliente(object_state.client_id());

    // creo el audio necesito SDL ya inicializado
    const std::string resourcesRoot = std::string(CLIENT_ASSETS_DIR) + "/../resources";
    gestorAudio = std::make_unique<GestorAudio>(resourcesRoot + "/config/sonidos.toml",
                                                resourcesRoot);
    gestorAudio->reproducirMusica("campo");

    // const uint32_t frame_target_ms = 1000u / static_cast<uint32_t>(config.fpsMax);
    // uint32_t next_game_tick = SDL_GetTicks();
    //
    // int update_counter = 0;
    // is_running = true;
    // while (is_running) {
    //     handleEvents();
    //     int loops = 0;
    //     uint32_t current_tick = SDL_GetTicks();
    //     while (current_tick >= next_game_tick && loops < MAX_FRAMESKIP) {
    //         update(update_counter);
    //         next_game_tick += frame_target_ms;
    //         ++update_counter;
    //         ++loops;
    //         current_tick = SDL_GetTicks();
    //     }
    //
    //     // Drop: si seguimos atrasados despues del maximo de updates, soltamos ticks
    //     // viejos para no ralentizar la simulacion intentando recuperar todo.
    //     if (loops == MAX_FRAMESKIP && current_tick >= next_game_tick) {
    //         next_game_tick = current_tick + frame_target_ms;
    //     }
    //
    //     render();
    //
    //     if (!config.vsync) {
    //         current_tick = SDL_GetTicks();
    //         if (current_tick < next_game_tick) {
    //             SDL_Delay(next_game_tick - current_tick);
    //         }
    //     }
    // }
    const uint32_t frame_target_ms = 1000 / static_cast<uint32_t>(config.fpsMax);
    int it = 0;
    uint32_t tick = SDL_GetTicks();
    is_running = true;
    while (is_running) {
        handleEvents();
        update(it);
        render();
        const uint32_t tick2 = SDL_GetTicks();
        int rest = static_cast<int>(frame_target_ms) - static_cast<int>(tick2 - tick);
        if (rest < 0) {
            const int behind = -rest;
            rest = frame_target_ms - (behind % frame_target_ms);
            const int lost = behind + rest;
            tick += lost;
            it += lost / frame_target_ms;
        } else {
            SDL_Delay(rest);
        }
        tick +=  frame_target_ms;
        it ++;
    }
}

void ClientGameLoop::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        const ResultadoInput resultado = handler.handle_event(event, object_state.entities());
        if (resultado.comando) {
            despacharComando(*resultado.comando, SDL_GetTicks());
        }
        if (resultado.lineaChat) {
            procesarLineaChat(*resultado.lineaChat, SDL_GetTicks());
        }
    }

    if (handler.should_quit()) {
        is_running = false;
    }
}

void ClientGameLoop::procesarLineaChat(const std::string& linea, const uint32_t current_tick) {
    const ResultadoParseo resultado = parser.parsear(linea, handler.objetivoSeleccionado());
    if (resultado.comando) {
        despacharComando(*resultado.comando, current_tick);
    } else if (!resultado.error.empty()) {
        std::cerr << "[chat] " << resultado.error << std::endl;
    }
}

void ClientGameLoop::despacharComando(const ComandoJugador& command, const uint32_t current_tick) {
    if (auto action = animation_action_for_command(command)) {
        object_animation.on_action(*action);
        object_state.notify_move_requested(current_tick);
    }
    reproducirSonidoDeComando(command);
    business.save_command(command);
}

void ClientGameLoop::reproducirSonidoDeComando(const ComandoJugador& command) {
    if (!gestorAudio) {
        return;
    }
    switch (command.opcode) {
        case Opcode::COMPRAR:
            gestorAudio->reproducirEfecto("comercianteComprar");
            break;
        case Opcode::VENDER:
            gestorAudio->reproducirEfecto("comercianteVender");
            break;
        case Opcode::LISTAR:
            gestorAudio->reproducirEfecto("comercianteBienvenida");
            break;
        case Opcode::CURAR:
            gestorAudio->reproducirEfecto("sacerdoteInteraccion");
            break;
        case Opcode::DEPOSITAR_ITEM:
        case Opcode::DEPOSITAR_ORO:
        case Opcode::RETIRAR_ITEM:
        case Opcode::RETIRAR_ORO:
            gestorAudio->reproducirEfecto("banqueroBienvenida");
            break;
        default:
            break;
    }
}

void ClientGameLoop::update(const int it) {
    const uint32_t current_tick = SDL_GetTicks();
    object_state.upload_server_msg(server_messages, current_tick, *gestorAudio);
    object_renderer.update_animation(it, object_state, object_animation);
}

void ClientGameLoop::render() {
    object_renderer.render(object_state, object_animation);
}

void ClientGameLoop::clean() {}

bool ClientGameLoop::isRunning() const {
    return is_running;
}

std::optional<GameAction> ClientGameLoop::animation_action_for_command(
        const ComandoJugador& command) const {
    if (command.opcode != Opcode::EMPEZAR_MOVER) {
        return std::nullopt;
    }

    const auto* move = std::get_if<ComandoEmpezarMover>(&command.payload);
    if (move == nullptr) {
        return std::nullopt;
    }

    switch (move->direccion) {
        case 0:
            return GameAction::MoveUp;
        case 1:
            return GameAction::MoveDown;
        case 2:
            return GameAction::MoveLeft;
        case 3:
            return GameAction::MoveRight;
        default:
            return std::nullopt;
    }
}
