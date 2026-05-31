#ifndef DATO_SESION_CLIENTE_H
#define DATO_SESION_CLIENTE_H

#include <string>
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

struct handshakeInicial {
    bool crearPersonaje = false;
    std::string nombre;
    ClasePersonaje clasePersonaje;
    Raza raza;
    uint16_t cabeza = 0;
    uint16_t cuerpo = 0;
};

#endif
