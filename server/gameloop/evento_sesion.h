#ifndef EVENTO_SESION_H
#define EVENTO_SESION_H

#include <cstdint>
#include <string>

#include "../game/modelo/clase_personaje.h"
#include "../../common/game/modelo/posicion.h"
#include "../game/modelo/raza.h"

enum class TipoEventoSesion { Conectar, Desconectar };

struct DatosSesion {
    std::string nombre;
    ClasePersonaje clase;
    Raza raza;
    Posicion posicion;
};

struct EventoSesion {
    TipoEventoSesion tipo;
    uint16_t idCliente;
    DatosSesion datos;
};

#endif
