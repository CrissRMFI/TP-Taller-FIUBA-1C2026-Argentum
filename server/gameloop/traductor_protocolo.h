#ifndef TRADUCTOR_PROTOCOLO_H
#define TRADUCTOR_PROTOCOLO_H

#include "../game/evento/evento_salida.h"
#include "mensaje_salida.h"

// Convierte `EventoSalida` (capa de dominio) en `MensajeSalida` (capa de
// protocolo). Es la única clase del servidor que conoce simultáneamente
// los tipos del modelo y los Opcodes del wire format. `Juego` no depende
// de ella; el gameloop la usa justo antes de pasar al `MonitorClientes`.
class TraductorProtocolo {
public:
    static MensajeSalida traducir(const EventoSalida& evento);
};

#endif
