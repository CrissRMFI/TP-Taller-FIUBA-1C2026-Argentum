#ifndef TALLER_TP_ESTADO_PANEL_RENDER_H
#define TALLER_TP_ESTADO_PANEL_RENDER_H

#include <cstdint>
#include <string>
#include <vector>

#include <SDL_pixels.h>

#include "client_game_world.h"

struct ConfigPanelRender {
    int ancho = 268;
    std::string iconDir = "imgs/items";
    std::string fondoCuero = "imgs/ui/leather_brown.png";
    std::string marcoInventario = "imgs/ui/es_centroinventario.bmp";
    std::string botonVender = "imgs/ui/es_boton-vender-default.bmp";
    std::string botonEquipar = "imgs/ui/es_boton-construir-default.bmp";
    std::string barraVida = "imgs/ui/en_barradevida.bmp";
    std::string barraMana = "imgs/ui/en_barrademana.bmp";
    SDL_Color   colorTexto = {235, 225, 200, 255};
    SDL_Color   colorTitulo = {255, 220, 120, 255};
};


struct EstadoPanelRender {
    std::vector<uint16_t> inventario;
    EquipamientoJugador equip;
    EstadoJugador stats;
    std::vector<uint16_t> stock;
    int seleccionInventario = -1;
    int scrollStock = 0;
};

#endif
