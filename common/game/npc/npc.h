#ifndef NPC_H
#define NPC_H

#include <cstdint>

#include "../modelo/posicion.h"

enum class TipoNpc { Banquero, Comerciante, Sacerdote };

class Npc {
private:
    uint16_t id;
    TipoNpc tipo;
    Posicion posicion;

public:
    Npc(uint16_t id, TipoNpc tipo, Posicion posicion);

    uint16_t getId() const;
    TipoNpc getTipo() const;
    Posicion getPosicion() const;
};

#endif
