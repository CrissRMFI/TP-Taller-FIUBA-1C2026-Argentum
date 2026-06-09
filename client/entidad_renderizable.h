#ifndef TALLER_TP_ENTIDAD_RENDERIZABLE_H
#define TALLER_TP_ENTIDAD_RENDERIZABLE_H

#include <cstdint>

struct EntidadRenderizable {
    uint16_t x;
    uint16_t y;
    uint8_t tipo;
    uint8_t estado;
    uint16_t cabeza; // Cabeza de una criatura es 0, solo se renderiza el cuerpo
    uint16_t cuerpo;
    uint16_t arma = 0;    // sprite del arma/baculo equipado (overlay; 0 = ninguno)
    uint16_t escudo = 0;  // sprite del escudo equipado (overlay; 0 = ninguno)
    uint16_t casco = 0;   // sprite del casco equipado (overlay; 0 = ninguno)
};

#endif
