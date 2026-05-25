#ifndef DATOS_CONEXION_H
#define DATOS_CONEXION_H

#include <string>
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"


struct DatosLogin {
    std::string puerto;
    std::string host;
};

struct DatosNuevoPersonaje {
    std::string nick;
    std::string password;
    Raza raza;
    ClasePersonaje clase;
};

struct DatosPersonaje {
    std::string nick;
    std::string password;
};

#endif