#ifndef MENSAJE_SALIDA_H
#define MENSAJE_SALIDA_H

#include <cstdint>

#include "../../common/protocolo/mensaje_servidor.h"

enum class TipoDestino {
    UNO,
    TODOS
};

struct MensajeSalida {
    TipoDestino tipoDestino;
    uint16_t idCliente;
    MensajeServidor mensaje;
};

#endif
