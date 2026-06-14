#ifndef PORTAL_H
#define PORTAL_H

#include "../modelo/posicion.h"

// Un Portal vincula una celda de un mapa con una celda de otro (o del mismo).
struct Portal {
    Posicion origen;
    Posicion destino;
};

#endif
