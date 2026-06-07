#ifndef TALLER_TP_ESTADO_CHAT_RENDER_H
#define TALLER_TP_ESTADO_CHAT_RENDER_H

#include <string>
#include <vector>

#include <SDL_pixels.h>

/* Configuracion ESTATICA del chat: sale del TOML y no cambia entre frames. Se le pasa al renderer una sola vez en init(). El ancho de la caja no esta aca: lo deriva el
renderer como la mitad del ancho de la ventana.
*/ 
struct ConfigChatRender {
    std::string fuenteRuta;
    int         fuenteTam = 14;
    std::string fondoRuta;
    int         panelX = 0;
    int         panelY = 0;
    int         panelAlto = 0;
    SDL_Color   colorTexto = {255, 255, 255, 255};
    SDL_Color   colorInput = {255, 255, 0, 255};
};

// Estado DINAMICO del chat en un frame: lo arma el loop a partir del input handler y del world. Es lo unico que cambia frame a frame.
struct EstadoChatRender {
    bool activo = false; // hay foco para escribir
    std::string entrada; // texto que se esta tipeando
    std::vector<std::string> historial; // mensajes entrantes (mas viejo -> mas nuevo)
};

#endif
