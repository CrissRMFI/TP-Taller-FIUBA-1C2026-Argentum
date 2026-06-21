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
    bool clickVacio = false;  // click en el area de juego que no selecciono nada
};

class ClientInputHandler {
private:
    bool quit_requested;
    int window_width;
    int window_height;
    uint16_t idCliente;
    bool chat_activo;
    std::string chat_buffer;
    std::optional<uint16_t> objetivo_seleccionado;
    int initial_window_width = 0;
    float panel_width_ratio = 0.26f;
    // Caja del chat (px). El ancho es la mitad de la ventana (igual que el renderer).
    int chat_panel_x = 0;
    int chat_panel_y = 0;
    int chat_panel_alto = 0;
    // Ancho del panel derecho (px). El area de juego es window_width - ancho_panel.
    int ancho_panel = 0;
    // Dimensiones del mapa (celdas) y radio de seleccion (px), desde el TOML.
    int mapa_ancho = 100;
    int mapa_alto = 100;
    float radio_seleccion_px = 40.0f;
    // Transform actual de la camara (lo empuja el loop cada frame). Si cam_tile_w<=0
    // todavia no esta seteado y el hit-test usa el calculo viejo (mapa completo).
    int cam_off_x = 0;
    int cam_off_y = 0;
    int cam_tile_w = 0;
    int cam_tile_h = 0;

    std::optional<ComandoJugador> handle_keyboard(SDL_Keycode key);
    // Direccion de protocolo (0=N,1=S,2=O,3=E) si la tecla es de movimiento.
    std::optional<uint8_t> direccion_de_tecla(SDL_Keycode key) const;
    bool click_en_chat(int x, int y) const;
    int ancho_panel_actual() const;
    int ancho_chat_actual() const;
    int ancho_juego() const;
    ResultadoInput manejar_texto_chat(const SDL_Event& event);
    void abrir_chat();
    void cerrar_chat();
    std::optional<ComandoJugador> handle_mouse_click(
            int x, int y, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);
    uint16_t buscar_id_objetivo(
            int x, int y, const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) const;

public:
    ClientInputHandler();
    ~ClientInputHandler();

    ResultadoInput handle_event(const SDL_Event& event,
                                const std::unordered_map<uint16_t, EntidadRenderizable>& entidades);

    bool should_quit() const;
    bool chatActivo() const {
        return chat_activo;
    }
    const std::string& bufferChat() const {
        return chat_buffer;
    }
    std::optional<uint16_t> objetivoSeleccionado() const {
        return objetivo_seleccionado;
    }
    // Id de la entidad interactuable bajo una posicion de pantalla (0 = ninguna).
    // Se usa para el hover (resaltar lo clickeable). No selecciona nada.
    uint16_t idEntidadEn(int x, int y,
                         const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) const {
        return buscar_id_objetivo(x, y, entidades);
    }

    void setIdCliente(uint16_t id) {
        idCliente = id;
    }
    void set_window_dimensions(int width, int height) {
        if (initial_window_width <= 0 && width > 0) {
            initial_window_width = width;
        }
        window_width = width;
        window_height = height;
    }
    void setChatPanel(int x, int y, int alto) {
        chat_panel_x = x;
        chat_panel_y = y;
        chat_panel_alto = alto;
    }
    void setAnchoPanel(int ancho) {
        ancho_panel = ancho;
        if (initial_window_width > 0 && ancho > 0) {
            panel_width_ratio = static_cast<float>(ancho) / static_cast<float>(initial_window_width);
        }
    }
    void setMapaDimensiones(int ancho, int alto) {
        if (ancho > 0)
            mapa_ancho = ancho;
        if (alto > 0)
            mapa_alto = alto;
    }
    void setRadioSeleccion(float radioPx) {
        if (radioPx > 0.0f)
            radio_seleccion_px = radioPx;
    }
    void setCamaraTransform(int offX, int offY, int tileW, int tileH) {
        cam_off_x = offX;
        cam_off_y = offY;
        cam_tile_w = tileW;
        cam_tile_h = tileH;
    }
};


#endif
