#ifndef EVENTO_SESION_H
#define EVENTO_SESION_H

#include <cstdint>
#include <string>

#include "../game/modelo/clase_personaje.h"
#include "../game/modelo/raza.h"

enum class TipoEventoSesion { Conectar, Desconectar };

// Datos que la capa de red entrega al dominio al abrir una sesion. No incluye Posicion: la spawn la decide Juego leyendo su ConfigJuego.
struct DatosSesion {
    std::string nombre;
    ClasePersonaje clase;
    Raza raza;
    uint16_t cabeza;
    uint16_t cuerpo;
};

struct EventoSesion {
    TipoEventoSesion tipo;
    uint16_t idCliente;
    DatosSesion datos;
};

#endif
