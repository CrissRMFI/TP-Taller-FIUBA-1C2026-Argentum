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

using PayloadMensajeServidor = std::variant<
        MensajeEstadoPersonaje>;


struct MensajeServidor {
    Opcode         opcode;
    PayloadMensajeServidor payload;
};


#endif
