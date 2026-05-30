//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_INPUT_HANDLER_H
#define TALLER_TP_CLIENT_INPUT_HANDLER_H

#include <optional>
#include <unordered_map>

#include <SDL.h>

#include "../../common/protocolo/comando_jugador.h"
#include "../entidad_renderizable.h"

class ClientInputHandler {
private:
    bool quit_requested;
    int window_width;
    int window_height;
    uint16_t idCliente;
    std::optional<ComandoJugador> handle_keyboard(SDL_Keycode key);
    std::optional<ComandoJugador> handle_mouse_click(int x, int y, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);
    uint16_t buscar_id_objetivo(int x, int y, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);

public:
    ClientInputHandler();
    ~ClientInputHandler();

    std::optional<ComandoJugador> handle_event(const SDL_Event &event, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);
    bool should_quit() const;
    void setIdCliente(uint16_t id) { idCliente = id; }
    void set_window_dimensions(int width, int height) {
        window_width = width;
        window_height = height;
    }
};


#endif //TALLER_TP_CLIENT_INPUT_HANDLER_H
