#ifndef COMANDO_CLIENTE_H
#define COMANDO_CLIENTE_H

#include <cstdint>

#include "../../common/protocolo/comando_jugador.h"

struct ComandoCliente {
    uint16_t idCliente;
    ComandoJugador comando;
};

#endif
