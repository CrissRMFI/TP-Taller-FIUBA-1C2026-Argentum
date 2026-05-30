//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_input_handler.h"

#include <algorithm>
#include <cmath>
#include <iostream>

ClientInputHandler::ClientInputHandler(): quit_requested(false), window_width(0), window_height(0) {}

ClientInputHandler::~ClientInputHandler() = default;

 std::optional<ComandoJugador> ClientInputHandler::handle_keyboard(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE:
            quit_requested = true;
            return std::nullopt;
        case SDLK_UP:    case SDLK_w: return ComandoJugador{Opcode::MOVER, ComandoMover{0}};
        case SDLK_DOWN:  case SDLK_s: return ComandoJugador{Opcode::MOVER, ComandoMover{1}};
        case SDLK_LEFT:  case SDLK_a: return ComandoJugador{Opcode::MOVER, ComandoMover{2}};
        case SDLK_RIGHT: case SDLK_d: return ComandoJugador{Opcode::MOVER, ComandoMover{3}};
        case SDLK_r:                  return ComandoJugador{Opcode::RESUCITAR, ComandoResucitar{}};
        case SDLK_m:                  return ComandoJugador{Opcode::MEDITAR, ComandoMeditar{}};
        default: return std::nullopt;
    }
}

std::optional<ComandoJugador> ClientInputHandler::handle_event(const SDL_Event &event, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {
    if (event.type == SDL_QUIT) {
        quit_requested = true;
    }
    if (event.type == SDL_KEYDOWN) {
        if (event.key.repeat != 0) {
            return std::nullopt;
        }
        return handle_keyboard(event.key.keysym.sym);
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        return handle_mouse_click(event.button.x, event.button.y, entidades);
    }
    return std::nullopt;

}

std::optional<ComandoJugador> ClientInputHandler::handle_mouse_click(int x, int y, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {
    uint16_t idObjetivo = buscar_id_objetivo(x, y, entidades);
    if (idObjetivo == 0) {
        return std::nullopt;
    }
    return ComandoJugador{Opcode::ATACAR, ComandoAtacar{idObjetivo}}; // TODO: calcular idObjetivo a partir de x e y
}

uint16_t ClientInputHandler::buscar_id_objetivo(int x, int y, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {

    uint16_t idObjetivo = 0;

    float tamanoCeldaX = static_cast<float>(window_width)  / 100.0f;
    float tamanoCeldaY = static_cast<float>(window_height) / 100.0f;

    // Radio = mitad de la diagonal de la celda, mínimo 25px
    float radioJusto = std::sqrt(tamanoCeldaX * tamanoCeldaX + tamanoCeldaY * tamanoCeldaY) / 2.0f;
    const float radioSeleccion = std::max(25.0f, radioJusto);
    float mejorDistancia = radioSeleccion + 1.0f;

    for (const auto& [id, entidad] : entidades) {
        if (id == idCliente) continue;

        // Centro del sprite en píxeles
        float entidadX = (static_cast<float>(entidad.x) + 0.5f) * tamanoCeldaX;
        float entidadY = (static_cast<float>(entidad.y) + 0.5f) * tamanoCeldaY;

        float dx = entidadX - static_cast<float>(x);
        float dy = entidadY - static_cast<float>(y);
        float distancia = std::sqrt(dx * dx + dy * dy);

        if (distancia <= radioSeleccion && distancia < mejorDistancia) {
            mejorDistancia = distancia;
            idObjetivo = id;
        }
    }
    if (idObjetivo != 0) {
        std::cout << "Click en (" << x << ", " << y << ") -> id objetivo seleccionado: " << idObjetivo << std::endl;
    }
    return idObjetivo;
}

bool ClientInputHandler::should_quit() const { return quit_requested; }
