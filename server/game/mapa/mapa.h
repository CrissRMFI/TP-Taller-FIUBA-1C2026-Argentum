#ifndef MAPA_H
#define MAPA_H

#include "../modelo/posicion.h"

class Mapa {
public:
    bool posicionValida(const Posicion& posicion) const;
    bool hayParedEn(const Posicion& posicion) const;
};

#endif
