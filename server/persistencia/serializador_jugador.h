#ifndef SERVER_PERSISTENCIA_SERIALIZADOR_JUGADOR_H
#define SERVER_PERSISTENCIA_SERIALIZADOR_JUGADOR_H

#include <cstdint>

#include "../../common/persistencia/formato_jugador.h"
#include "../game/jugador.h"

class SerializadorJugador {
public:
    static RegistroJugador aRegistro(const Jugador& jugador);

    static Jugador aJugador(uint16_t sessionId,
                            const RegistroJugador& registro,
                            const ConfigJuego& cfg);
};

#endif
