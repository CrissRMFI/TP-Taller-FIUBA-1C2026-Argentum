//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_game_loop.h"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <variant>

#include "../common/mensajes/mensajes_error_accion.h"

using namespace SDL2pp;

ClientGameLoop::ClientGameLoop(Queue<ComandoJugador>& outgoing_commands,
                               Queue<MensajeServidor>& server_messages,
                               uint16_t idCliente):
    commands_queue(outgoing_commands),
    server_messages(server_messages),
    business(outgoing_commands),
    idCliente(idCliente),
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
    IMG_Init(IMG_INIT_PNG);
    window_width = width;
    window_height = height;

    SDL_Surface* surf = IMG_Load("client/resources/pasto.png");
    if (surf) {
        background_texture = SDL_CreateTextureFromSurface(renderer.Get(), surf);
        SDL_FreeSurface(surf);
    } else {
        std::cerr << "No se pudo cargar la imagen de fondo: " << IMG_GetError() << std::endl;
    }
    handler.set_window_dimensions(width, height);
    handler.setIdCliente(idCliente);
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
        std::optional<ComandoJugador> comando = handler.handle_event(event, entidades);
        if (comando) {
            business.save_command(*comando);
        }
    }
    if (handler.should_quit()) {
        is_running = false;
    }
}

// ver como se actualizan los comandos
void ClientGameLoop::update() {
    // ComandoJugador command;
    // while (commands_queue.try_pop(command)) {
    //     if (command.opcode == Opcode::MOVER) {
    //         const auto move = std::get<ComandoMover>(command.payload);
    //         std::cout << "Move command queued. direction=" << static_cast<int>(move.direccion)
    //                   << std::endl;
    //     }
    // }

    MensajeServidor mensaje;
    while (server_messages.try_pop(mensaje)) {
        if (auto* p = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            entidades[p->id] = EntidadRenderizable{p->x, p->y, p->tipo, p->estado};
        } else if (auto* p = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(p->id);
        } else if (auto* p = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            entidades.erase(p->id);
        } else if (auto* p = std::get_if<MensajeEstadoPersonaje>(&mensaje.payload)) {
            std::cout << "Recibido estado personaje: vida " << p->vidaActual << "/" << p->vidaMax
                      << ", mana " << p->manaActual << "/" << p->manaMax
                      << ", oro " << p->oro
                      << ", nivel " << static_cast<int>(p->nivel)
                      << ", experiencia " << p->experiencia
                      << ", estado " << static_cast<int>(p->estado)
                      << std::endl;

        } else if (auto* p = std::get_if<MensajeDanoRecibido>(&mensaje.payload)) {
            std::cout << "Cliente actual (" << idCliente << ") recibió daño: cantidad " << p->cantidad << ", id atacante " << p->idAtacante << std::endl;

        } else if (auto* p = std::get_if<MensajeDanoProducido>(&mensaje.payload)) {
            std::cout << "Cliente actual (" << idCliente << ") produjo daño: cantidad " << p->cantidad << ", id objetivo " << p->idObjetivo << std::endl;

        } else if (auto* p = std::get_if<MensajeEsquive>(&mensaje.payload)) {
            std::cout << "Cliente actual (" << idCliente << ") esquivó un ataque: id entidad " << p->idEntidad << ", esquivador " << static_cast<int>(p->esquivador) << std::endl;

        } else if (auto* p = std::get_if<MensajeResucitado>(&mensaje.payload)) {
            std::cout << "Cliente actual (" << idCliente << ") resucitado en posición (" << p->x << ", " << p->y << ")" << std::endl;
        
        } else if (auto* p = std::get_if<MensajeErrorAccion>(&mensaje.payload)) {
            std::cout << "Error en acción realizada por cliente actual (" << idCliente << "): código error " << static_cast<int>(p->codigo) << std::endl;
        
        } else if (auto* p = std::get_if<MensajeItemEnSuelo>(&mensaje.payload)) {
            std::cout << "Item en suelo: id item " << p->idItem << ", posición (" << p->x << ", " << p->y << ")" << std::endl;
        
        } else if (auto* p = std::get_if<MensajeOroEnSuelo>(&mensaje.payload)) {
            std::cout << "Oro en suelo: cantidad " << p->cantidad << ", posición (" << p->x << ", " << p->y << ")" << std::endl;

        } else if (auto* p = std::get_if<MensajeItemDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "Item desapareció del suelo: posición (" << p->x << ", " << p->y << ")" << std::endl;
        
        } else if (auto* p = std::get_if<MensajeOroDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "Oro desapareció del suelo: posición (" << p->x << ", " << p->y << ")" << std::endl;
        }

        // El resto de mensajes (estado personaje, inventario, chat, oro/items en suelo, etc.) no afectan el render del mundo todavia; los procesaremos cuando agreguemos esas mecanicas al cliente.
    }
}



void ClientGameLoop::render(SDL2pp::Renderer& renderer) {
    if (background_texture) {
        SDL_Rect dst{0, 0, window_width, window_height};
        SDL_RenderCopy(renderer.Get(), background_texture, nullptr, &dst);
    } else {
        renderer.SetDrawColor(0, 255, 0, 255);
        renderer.Clear();
    }
    for (const auto& [id, e] : entidades) {
        renderer.SetDrawColor(elegircolor(e.tipo, e.estado));
        renderer.FillRect(Rect(e.x * window_width / 100, e.y * window_height / 100, window_width / 100, window_height / 100));
    }

    renderer.Present();
}

void ClientGameLoop::clean() {
    if (background_texture) {
        SDL_DestroyTexture(background_texture);
        background_texture = nullptr;
    }
    IMG_Quit();
}

bool ClientGameLoop::isRunning() const {
    return is_running;
}

SDL_Color ClientGameLoop::elegircolor(uint8_t tipo, uint8_t estado) {
    if (tipo == 0) { // personaje
        switch (estado) {
            case 0: return {0, 0, 255, 255}; // vivo: azul
            case 1: return {128, 128, 128, 255}; // fantasma: gris
            case 2: return {255, 255, 0, 255}; // meditando: amarillo 
        }
    } else if (tipo == 1) { // criatura
        switch (estado) {
            case 0: return {255, 0, 0, 255}; // viva: rojo
        }
    }
    return {255, 255, 255, 255}; // blanco por defecto
}