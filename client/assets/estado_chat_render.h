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
    // lineas de ayuda de comandos (se muestran al abrir)
    std::vector<std::string> ayuda;
    // colores (vienen del TOML via ConfigCliente)
    SDL_Color colorTexto = {255, 255, 255, 255};
    SDL_Color colorInput = {255, 255, 0, 255};
    SDL_Color colorAyuda = {200, 200, 200, 255};
    // caja del chat en pixeles (viene del TOML via ConfigCliente)
    int panelX = 8;
    int panelY = 8;
    int panelAncho = 380;
    int panelAlto = 120;
};

#endif
