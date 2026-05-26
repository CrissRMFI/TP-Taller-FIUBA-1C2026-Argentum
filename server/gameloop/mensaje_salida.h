#ifndef MENSAJE_SALIDA_H
#define MENSAJE_SALIDA_H

#include <cstdint>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../game/evento/tipo_destino.h"

struct MensajeSalida {
    TipoDestino tipoDestino;
    uint16_t idCliente;
    MensajeServidor mensaje;
};

#endif
