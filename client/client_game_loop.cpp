//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_game_loop.h"

#include <SDL.h>

#include <iostream>
#include <variant>
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Surface.hh"
#include "SDL2pp/Texture.hh"
#include "SDL_image.h"
#include "assets/sprite_manager.h"
#include "server/gameloop/comando_cliente.h"

using namespace SDL2pp;

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
    sdl = std::make_unique<SDL>(SDL_INIT_VIDEO);
    SDL2pp::SDLImage imageInit(IMG_INIT_PNG);
    window = std::make_unique<Window>(title, xpos, ypos, width, height, flags);
    renderer = std::make_unique<Renderer>(*window, -1, SDL_RENDERER_ACCELERATED);
    try {
        // Carga de texturas y Sprites
        SDL2pp::Surface surface("client/assets/imgs/1009.png");
        texture = std::make_unique<Texture>(*renderer, surface);
        std::array<int, 4> frames_ = {6, 6, 5, 5};
        sprite_manager = std::make_unique<SpriteManager>(*texture, 32, 40, frames_, 12);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar la imagen: " << e.what() << std::endl;
    }
    window->Raise();

    is_running = true;
    while (is_running) {
        handleEvents();
        update();
        render();
        SDL_Delay(10);
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
void ClientGameLoop::update() {
    bool moving = false;
    ComandoJugador comando;
    while (commands_queue.try_pop(comando)) {
        if (comando.opcode == Opcode::MOVER) {
            const auto move = std::get<ComandoMover>(comando.payload);
          //  std::cout << "Comando mover en queue. Direccion = " << static_cast<int>(move.direccion) << std::endl;
            switch (move.direccion) {
                case 0:
                    current_fila = 0;
                    break;
                case 1:
                    current_fila = 1;
                    break;
                case 2: current_fila = 2;
                    break;
                case 3:
                    current_fila = 3;
                    break;
                default:
                    sprite_manager->reset_frame();
            }
            moving = true;
        }
    }

    MensajeServidor mensaje;
    while (server_messages.try_pop(mensaje)) {
        if (auto* p = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            entidades[p->id] = EntidadRenderizable{p->x, p->y, p->tipo, p->estado};

            // Si el mensaje del servidor nos pertenece, actualizamos nuestra posición absoluta
            if (p->id == idCliente) {
                posx = p->x;
                posy = p->y;
            }

            std::cout << "[cliente] entidad " << p->id
                      << " en (" << p->x << ", " << p->y << ")"
                      << " tipo=" << static_cast<int>(p->tipo)
                      << " estado=" << static_cast<int>(p->estado) << std::endl;

        }
        // else if (auto* entidad_desaparecio = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
        //     entidades.erase(entidad_desaparecio->id);
        //     std::cout << "[cliente] entidad " << entidad_desaparecio->id << " desaparecio" << std::endl;
        // } else if (auto* muerte_entidad = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
        //     entidades.erase(muerte_entidad->id);
        //     std::cout << "[cliente] entidad " << muerte_entidad->id << " murio" << std::endl;
        // }
    }

    if (sprite_manager) {
        if (moving) {
            sprite_manager->update(SDL_GetTicks(), current_fila);
        } else {
            sprite_manager->reset_frame();
        }
    }
}

void ClientGameLoop::render() {
    if (!renderer) return;

    // Limpieza de pantalla (Fondo gris oscuro)
    renderer->SetDrawColor(30, 30, 30, 255);
    renderer->Clear();

    // Dibujamos todas las entidades guardadas en nuestro mapa
    for (const auto& [id, e] : entidades) {
        // NOTA: Si e.x y e.y representan celdas de una grilla (ej: celda 10, 15),
        // deberás multiplicarlos por el tamaño de tu celda (ej: e.x * TAMANIO_CELDA)

        if (id == idCliente) {
            // Si es nuestro propio jugador, lo dibujamos con su SpriteManager animado
            if (sprite_manager) {
                sprite_manager->render(*renderer, posx, posy, current_fila, 2.0f);
            }
        }
        // } else {
        //     // Para otras entidades (NPCs, enemigos u otros jugadores) de momento dibujamos un placeholder
        //     renderer->SetDrawColor(200, 50, 50, 255); // Color rojo tentativo
        //     SDL_Rect rect = { static_cast<int>(e.x), static_cast<int>(e.y), 32, 32 };
        //     renderer->FillRect(rect);
        // }
    }

    renderer->Present();
}

bool ClientGameLoop::isRunning() const {
    return is_running;
}

/*
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

    // Drenar mensajes del servidor y reflejarlos en el estado local de entidades.
    Es lo que dejara render() listo para dibujar en el proximo frame. El cliente solo
    refleja lo que el servidor dice; no decide nada.
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
        // El resto de mensajes (estado personaje, inventario, chat, oro/items en suelo, etc.)
        no afectan el render del mundo todavia; los procesaremos cuando agreguemos esas mecanicas al cliente.
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
*/