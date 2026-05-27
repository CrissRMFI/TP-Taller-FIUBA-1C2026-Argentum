//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_game_loop.h"

#include <SDL.h>

#include <iostream>
#include <variant>

using namespace SDL2pp;

ClientGameLoop::ClientGameLoop(Queue<ComandoJugador>& outgoing_commands,
                               Queue<MensajeServidor>& server_messages):
    commands_queue(outgoing_commands),
    server_messages(server_messages),
    business(outgoing_commands),
    is_running(false) {}

ClientGameLoop::~ClientGameLoop() = default;

void ClientGameLoop::init(const char* title, int xpos,
    int ypos, int width, int height, bool fullscreen) {
    uint32_t flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    SDL sdl(SDL_INIT_VIDEO);
    Window window(title, xpos, ypos, width, height, flags);
    Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
    window.Raise();
    is_running = true;
    while (is_running) {
        handleEvents();
        update();
        render(renderer);
    }
}

void ClientGameLoop::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (auto action = handler.handle_event(event)) {
            business.save_command(*action);
        }
    }

    if (handler.should_quit()) {
        is_running = false;
    }
}

// ver como se actualizan los comandos
void ClientGameLoop::update() {
    ComandoJugador command;
    while (commands_queue.try_pop(command)) {
        if (command.opcode == Opcode::MOVER) {
            const auto move = std::get<ComandoMover>(command.payload);
            std::cout << "Move command queued. direction=" << static_cast<int>(move.direccion)
                      << std::endl;
        }
    }

    // Drenar mensajes del servidor y reflejarlos en el estado local de entidades. Es lo que dejara render() listo para dibujar en el proximo frame. El cliente solo refleja lo que el servidor dice; no decide nada.
    MensajeServidor mensaje;
    while (server_messages.try_pop(mensaje)) {
        if (auto* p = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            entidades[p->id] = EntidadRenderizable{p->x, p->y, p->tipo, p->estado};
            std::cout << "[cliente] entidad " << p->id
                      << " en (" << p->x << ", " << p->y << ")"
                      << " tipo=" << static_cast<int>(p->tipo)
                      << " estado=" << static_cast<int>(p->estado) << std::endl;
        } else if (auto* p = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(p->id);
            std::cout << "[cliente] entidad " << p->id << " desaparecio" << std::endl;
        } else if (auto* p = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            entidades.erase(p->id);
            std::cout << "[cliente] entidad " << p->id << " murio" << std::endl;
        }
        // El resto de mensajes (estado personaje, inventario, chat, oro/items en suelo, etc.) no afectan el render del mundo todavia; los procesaremos cuando agreguemos esas mecanicas al cliente.
    }
}



void ClientGameLoop::render(SDL2pp::Renderer& renderer) {
    // Limpieza del frame: fondo gris parejo. Sin esto, la GPU muestra basura residual de otras apps en el framebuffer.
    renderer.SetDrawColor(30, 30, 30, 255);
    renderer.Clear();

    // TODO (proximo commit): dibujar las entidades del mapa `entidades`. Sketch:
    //   for (const auto& [id, e] : entidades) {
    //       renderer.SetDrawColor(<color segun tipo/estado>);
    //       renderer.FillRect(<rect en celda (e.x, e.y) * TAMANIO_CELDA>);
    //   }
    // A tener en cuenta:
    //   - elegir un TAMANIO_CELDA (ej. 16 px) y centrar el viewport en el propio jugador (regla 13.1.1, camara cenital)
    //   - distinguir tipo y estado (Personaje/Criatura, Vivo/Fantasma/Meditando)
    //   - mas adelante reemplazar FillRect por sprites con renderer.Copy(...)

    renderer.Present();
}

bool ClientGameLoop::isRunning() const {
    return is_running;
}
