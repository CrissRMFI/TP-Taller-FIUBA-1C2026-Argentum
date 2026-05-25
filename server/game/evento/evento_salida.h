#ifndef EVENTO_SALIDA_H
#define EVENTO_SALIDA_H

#include <cstdint>

#include "evento_juego.h"
#include "tipo_destino.h"

// Evento de juego con la información de routing (a qué cliente o conjunto de clientes le corresponde). Es lo que produce `Juego` y consume el `TraductorProtocolo` del gameloop.
struct EventoSalida {
    TipoDestino tipoDestino;
    uint16_t    idCliente;
    EventoJuego evento;
};

#endif
