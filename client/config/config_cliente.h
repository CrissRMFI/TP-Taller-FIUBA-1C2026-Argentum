#ifndef CLIENT_CONFIG_CONFIG_CLIENTE_H
#define CLIENT_CONFIG_CONFIG_CLIENTE_H

#include <cstdint>

struct ConfigCliente {
    bool     vsync = true;
    int      fpsMax = 60;
    uint32_t intervaloMovimientoMs = 130;

    int  ancho = 640;
    int  alto = 480;
    bool fullscreen = false;
};

#endif
