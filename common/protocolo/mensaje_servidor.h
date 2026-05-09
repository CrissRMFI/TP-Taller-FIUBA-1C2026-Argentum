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

struct MensajeServidor {
    Opcode         opcode;
    //Acá vamos a poner el payload de cada mensaje, que va a ser distinto dependiendo del opcode. Para eso tambien vamos a usar un std::variant.
};


#endif
