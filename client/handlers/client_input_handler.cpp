//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_input_handler.h"

#include <algorithm>
#include <cmath>

// Tope de caracteres del mini-chat (nuestro protocolo admite hasta 256).

#define MAX_CHARS_CHAT 200

ClientInputHandler::ClientInputHandler():
        quit_requested(false), window_width(0), window_height(0), idCliente(0),
        chat_activo(false) {}

ClientInputHandler::~ClientInputHandler() = default;

std::optional<ComandoJugador> ClientInputHandler::handle_keyboard(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE:
            quit_requested = true;
            return std::nullopt;
        case SDLK_m:
            return ComandoJugador{Opcode::MEDITAR, ComandoMeditar{}};
        case SDLK_r:
            return ComandoJugador{Opcode::RESUCITAR, ComandoResucitar{}};
        // Cheats de prueba (combinaciones de tecla): F1 vida infinita,
        // F2 mana infinito, F3 morir al instante.
        case SDLK_F1:
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::VidaInfinita)}};
        case SDLK_F2:
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::ManaInfinito)}};
        case SDLK_F3:
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::MorirAuto)}};
        default: return std::nullopt;
    }
}

void ClientInputHandler::abrir_chat() {
    chat_activo = true;
    chat_buffer.clear();
    SDL_StartTextInput();
}

void ClientInputHandler::cerrar_chat() {
    chat_activo = false;
    chat_buffer.clear();
    SDL_StopTextInput();
}

ResultadoInput ClientInputHandler::manejar_texto_chat(const SDL_Event& event) {
    ResultadoInput resultado;

    if (event.type == SDL_TEXTINPUT) {
        if (chat_buffer.size() < MAX_CHARS_CHAT) {
            chat_buffer += event.text.text;
        }
        return resultado;
    }

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER: {
                std::string linea = chat_buffer;
                cerrar_chat();
                if (!linea.empty()) {
                    resultado.lineaChat = std::move(linea);
                }
                break;
            }
            case SDLK_ESCAPE:
                cerrar_chat();
                break;
            case SDLK_BACKSPACE:
                if (!chat_buffer.empty()) {
                    chat_buffer.pop_back();
                }
                break;
            default:
                break;
        }
    }

    return resultado;
}

ResultadoInput ClientInputHandler::handle_event(
        const SDL_Event& event,
        const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {
    ResultadoInput resultado;

    if (event.type == SDL_QUIT) {
        quit_requested = true;
        return resultado;
    }

    if (chat_activo) {
        return manejar_texto_chat(event);
    }

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        const SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            abrir_chat();
            // Si veniamos moviendonos, frenamos: la tecla de direccion soltada
            // mientras se tipea no llegaria como DETENER.
            resultado.comando = ComandoJugador{Opcode::DETENER_MOVER, ComandoDetenerMover{}};
            return resultado;
        }
        // Tecla de direccion presionada: empezar a moverse (el server avanza solo).
        if (auto direccion = direccion_de_tecla(key)) {
            resultado.comando =
                    ComandoJugador{Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{*direccion}};
            return resultado;
        }
        resultado.comando = handle_keyboard(key);
        return resultado;
    }
    if (event.type == SDL_KEYUP) {
        // Tecla de direccion soltada: detener el movimiento.
        if (direccion_de_tecla(event.key.keysym.sym)) {
            resultado.comando = ComandoJugador{Opcode::DETENER_MOVER, ComandoDetenerMover{}};
        }
        return resultado;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        resultado.comando = handle_mouse_click(event.button.x, event.button.y, entidades);
        return resultado;
    }
    return resultado;
}

std::optional<uint8_t> ClientInputHandler::direccion_de_tecla(SDL_Keycode key) const {
    switch (key) {
        case SDLK_UP:    case SDLK_w: return 0;  // Norte
        case SDLK_DOWN:  case SDLK_s: return 1;  // Sur
        case SDLK_LEFT:  case SDLK_a: return 2;  // Oeste
        case SDLK_RIGHT: case SDLK_d: return 3;  // Este
        default: return std::nullopt;
    }
}

std::optional<ComandoJugador> ClientInputHandler::handle_mouse_click(
        int x, int y,
        const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {
    const uint16_t idObjetivo = buscar_id_objetivo(x, y, entidades);
    if (idObjetivo == 0) {
        return std::nullopt;
    }

    // El click selecciona a la entidad (para comandos de chat dirigidos a NPCs) y a la vez dispara un ataque sobre ella.
    objetivo_seleccionado = idObjetivo;
    return ComandoJugador{Opcode::ATACAR, ComandoAtacar{idObjetivo}};
}

uint16_t ClientInputHandler::buscar_id_objetivo(
        int x, int y,
        const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) const {
    if (window_width <= 0 || window_height <= 0) {
        return 0;
    }

    uint16_t idObjetivo = 0;
    const float tamanoCeldaX = static_cast<float>(window_width) / 100.0f;
    const float tamanoCeldaY = static_cast<float>(window_height) / 100.0f;
    const float radioJusto =
            std::sqrt(tamanoCeldaX * tamanoCeldaX + tamanoCeldaY * tamanoCeldaY) / 2.0f;
    const float radioSeleccion = std::max(25.0f, radioJusto);
    float mejorDistancia = radioSeleccion + 1.0f;

    for (const auto& [id, entidad] : entidades) {
        if (id == idCliente) {
            continue;
        }

        const float entidadX = (static_cast<float>(entidad.x) + 0.5f) * tamanoCeldaX;
        const float entidadY = (static_cast<float>(entidad.y) + 0.5f) * tamanoCeldaY;
        const float dx = entidadX - static_cast<float>(x);
        const float dy = entidadY - static_cast<float>(y);
        const float distancia = std::sqrt(dx * dx + dy * dy);

        if (distancia <= radioSeleccion && distancia < mejorDistancia) {
            mejorDistancia = distancia;
            idObjetivo = id;
        }
    }

    return idObjetivo;
}

bool ClientInputHandler::should_quit() const { return quit_requested; }
