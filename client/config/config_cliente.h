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
    int chatPanelX = 8;
    int chatPanelY = 8;
    int chatPanelAncho = 380;
    int chatPanelAlto = 120;
    std::vector<std::string> ayudaChat;
    std::vector<int> chatColorTexto = {255, 255, 255};
    std::vector<int> chatColorInput = {255, 255, 0};
    std::vector<int> chatColorAyuda = {200, 200, 200};
};

#endif
