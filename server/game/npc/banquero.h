#ifndef BANQUERO_H
#define BANQUERO_H

#include "npc.h"

#include <map>
#include <vector>
#include <algorithm>
#include <utility>

class Banquero : public Npc {
public:
    Banquero(uint16_t id, Posicion posicion) : Npc(id, TipoNpc::Banquero, posicion) {cuentas = {};}

    std::pair<uint32_t, std::vector<uint16_t>> listarItemsDisponibles(uint16_t idJugador);
    bool depositarOro(uint16_t idJugador, uint32_t cantidad);
    bool retirarOro(uint16_t idJugador, uint32_t cantidad);
    bool depositarItem(uint16_t idJugador, uint16_t idItem);
    bool retirarItem(uint16_t idJugador, uint16_t idItem);

private:
    std::map<uint16_t, std::pair<uint32_t, std::vector<uint16_t>>> cuentas; // <idJugador, <oro, <idItems>> >
};

#endif