#ifndef CLIENT_CONFIG_CONFIG_CLIENTE_H
#define CLIENT_CONFIG_CONFIG_CLIENTE_H

#include <cstdint>
#include <string>
#include <vector>

struct ConfigCliente {
    bool     vsync = true;
    int      fpsMax = 60;
    uint32_t intervaloMovimientoMs = 130;

    int  ancho = 640;
    int  alto = 480;
    bool fullscreen = false;

    std::string fuenteRuta = "fonts/font.ttf";
    std::string fondoChatRuta = "chat/fondoPrincipal.png";
    int fuenteTam = 14;
    int chatMaxLineas = 6;
    int chatPanelX = 0;
    int chatPanelY = 0;
    int chatPanelAlto = 120;  // el ancho lo deriva el renderer: mitad de la ventana
    std::vector<int> chatColorTexto = {255, 255, 255};
    std::vector<int> chatColorInput = {255, 255, 0};
};

#endif
