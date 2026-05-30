#ifndef DATO_SESION_CLIENTE_H
#define DATO_SESION_CLIENTE_H

#include <string>
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

struct handshakeInicial {
    bool crearPersonaje = false;
    std::string nombre;
    std::string password;
    ClasePersonaje clasePersonaje = ClasePersonaje::GUERRERO;
    Raza raza = Raza::HUMANO;
};

#endif
