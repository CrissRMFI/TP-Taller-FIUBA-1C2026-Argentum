//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_INPUT_HANDLER_H
#define TALLER_TP_CLIENT_INPUT_HANDLER_H

#include <optional>
#include <string>
#include <unordered_map>

#include <SDL.h>

#include "../../common/protocolo/comando_jugador.h"
#include "../entidad_renderizable.h"

// Resultado de procesar un evento de entrada. Un evento puede:
//  - producir un comando directo (atajo de teclado o click): 'comando'.
//  - enviar una linea escrita en el mini-chat: 'lineaChat' (la parsea el loop).
// Ambos pueden venir vacios (p.ej. al abrir el chat o tipear una letra).
struct ResultadoInput {
    std::optional<ComandoJugador> comando;
    std::optional<std::string> lineaChat;
};

class ClientInputHandler {
private:
    bool quit_requested;
    int window_width;
    int window_height;
    uint16_t idCliente;
    uint32_t last_move_tick;
    bool chat_activo;
    std::string chat_buffer;
    std::optional<uint16_t> objetivo_seleccionado;

    std::optional<ComandoJugador> handle_keyboard(SDL_Keycode key);
    ResultadoInput manejar_texto_chat(const SDL_Event& event);
    void abrir_chat();
    void cerrar_chat();
    std::optional<ComandoJugador> handle_mouse_click(
            int x, int y,
            const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);
    uint16_t buscar_id_objetivo(
            int x, int y,
            const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) const;

public:
    ClientInputHandler();
    ~ClientInputHandler();

    ResultadoInput handle_event(
            const SDL_Event& event,
            const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);

    std::optional<ComandoJugador> sondearMovimiento(uint32_t current_tick, uint32_t intervalo_ms);

    bool should_quit() const;
    bool chatActivo() const { return chat_activo; }
    const std::string& bufferChat() const { return chat_buffer; }
    std::optional<uint16_t> objetivoSeleccionado() const { return objetivo_seleccionado; }

    void setIdCliente(uint16_t id) { idCliente = id; }
    void set_window_dimensions(int width, int height) {
        window_width = width;
        window_height = height;
    }
};


#endif

