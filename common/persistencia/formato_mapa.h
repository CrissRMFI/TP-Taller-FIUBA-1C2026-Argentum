#ifndef COMMON_PERSISTENCIA_FORMATO_MAPA_H
#define COMMON_PERSISTENCIA_FORMATO_MAPA_H

#include <cstdint>

// Layout binario del .bin de mapa (un mapa por archivo):
//
//     [HeaderMapa]                              (24 bytes)
//     [ParedRecord   x header.cantParedes]      (04 bytes c/u)
//     [CiudadRecord  x header.cantCiudades]     (08 bytes c/u)
//     [NpcRecord     x header.cantNpcs]         (08 bytes c/u)
//
// Los primeros 4 bytes del archivo son el "magic" que identifica al formato
// (chars 'A','O','M','1' - Argentum Online Mapa version 1).

struct HeaderMapa {
    char     magic[4];
    uint16_t version;
    uint16_t mapaId;
    uint16_t ancho;
    uint16_t alto;
    uint32_t cantParedes;
    uint32_t cantCiudades;
    uint32_t cantNpcs;
};

struct ParedRecord {
    uint16_t x;
    uint16_t y;
};

struct CiudadRecord {
    uint16_t xMin;
    uint16_t yMin;
    uint16_t xMax;
    uint16_t yMax;
};

struct NpcRecord {
    uint16_t id;
    uint8_t  tipo;
    uint8_t  _padding;
    uint16_t x;
    uint16_t y;
};

// Los static_assert garantizan que el compilador no agrego padding extra:
static_assert(sizeof(HeaderMapa)   == 24);
static_assert(sizeof(ParedRecord)  == 4);
static_assert(sizeof(CiudadRecord) == 8);
static_assert(sizeof(NpcRecord)    == 8);

#endif
