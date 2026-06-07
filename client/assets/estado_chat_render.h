#ifndef TALLER_TP_ESTADO_CHAT_RENDER_H
#define TALLER_TP_ESTADO_CHAT_RENDER_H

#include <string>
#include <vector>

#include <SDL_pixels.h>

struct EstadoChatRender {
    // hay una linea abierta tipeandose
    bool activo = false;
    // texto que se esta escribiendo
    std::string entrada;
    // mensajes entrantes recientes (mas viejo -> mas nuevo)
    std::vector<std::string> historial;
    // colores (vienen del TOML via ConfigCliente). Estos defaults SI se usan: el loop
    // los pasa como fallback si el color del TOML viene mal formado.
    SDL_Color colorTexto = {255, 255, 255, 255};
    SDL_Color colorInput = {255, 255, 0, 255};
    // caja del chat en pixeles.
    int panelX = 0;
    int panelY = 0;
    int panelAncho = 0;
    int panelAlto = 0;
};

#endif
