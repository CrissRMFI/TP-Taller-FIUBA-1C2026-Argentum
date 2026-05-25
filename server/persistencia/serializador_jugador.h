#ifndef SERVER_PERSISTENCIA_SERIALIZADOR_JUGADOR_H
#define SERVER_PERSISTENCIA_SERIALIZADOR_JUGADOR_H

#include "../../common/persistencia/formato_jugador.h"

class Jugador;

class SerializadorJugador {
public:
    static RegistroJugador aRegistro(const Jugador& jugador);
};

#endif
