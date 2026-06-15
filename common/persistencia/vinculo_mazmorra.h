#ifndef COMMON_PERSISTENCIA_VINCULO_MAZMORRA_H
#define COMMON_PERSISTENCIA_VINCULO_MAZMORRA_H

#include <cstdint>
#include <string>

// Vinculo de un mapa con su mazmorra: la mazmorra vive en su propio archivo .toml
struct VinculoMazmorra {
    std::string archivo;

    // entrada: celda del exterior (x,y) -> aparece en (destinoX,destinoY) de la mazmorra.
    uint16_t entradaX = 0;
    uint16_t entradaY = 0;
    uint16_t entradaDestinoX = 0;
    uint16_t entradaDestinoY = 0;

    // salida: celda de la mazmorra (x,y) -> vuelve a (destinoX,destinoY) del exterior.
    uint16_t salidaX = 0;
    uint16_t salidaY = 0;
    uint16_t salidaDestinoX = 0;
    uint16_t salidaDestinoY = 0;
};

#endif
