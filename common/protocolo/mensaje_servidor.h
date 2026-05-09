#ifndef MENSAJE_SERVIDOR_H
#define MENSAJE_SERVIDOR_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "./opcode.h"

struct MensajeEstadoPersonaje {
    uint16_t vidaActual;
    uint16_t vidaMax;
    uint16_t manaActual;
    uint16_t manaMax;
    uint32_t oro;
    uint8_t  nivel;
    uint32_t experiencia;
};

struct MensajePosicionEntidad {
    uint16_t id;
    uint16_t x;
    uint16_t y;
    uint8_t tipo;
    uint8_t estado;
};

struct MensajeEntidadDesaparecio {
    uint16_t id;
};


using PayloadMensajeServidor = std::variant<
        MensajeEstadoPersonaje,
        MensajePosicionEntidad,
        MensajeEntidadDesaparecio>;


struct MensajeServidor {
    Opcode         opcode;
    PayloadMensajeServidor payload;
};


#endif
