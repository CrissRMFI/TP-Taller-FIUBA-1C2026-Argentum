#ifndef MENSAJE_SALIDA_H
#define MENSAJE_SALIDA_H

#include <cstdint>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../game/evento/tipo_destino.h"

// Wire-format ya listo para despachar por la red. Lo produce el
// `TraductorProtocolo` a partir de un `EventoSalida` del dominio.
struct MensajeSalida {
    TipoDestino tipoDestino;
    uint16_t idCliente;
    MensajeServidor mensaje;
};

#endif
