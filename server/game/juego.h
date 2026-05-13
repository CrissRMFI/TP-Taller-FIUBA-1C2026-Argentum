#ifndef JUEGO_H
#define JUEGO_H

#include <cstdint>
#include <list>

#include "../../common/protocolo/comando_jugador.h"
#include "../gameloop/mensaje_salida.h"

class Juego {
public:
    std::list<MensajeSalida> ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando);
};

#endif
