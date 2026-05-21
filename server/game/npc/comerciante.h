#ifndef COMERCIANTE_H
#define COMERCIANTE_H

#include "npc.h"

#include <vector>
#include <map>

class Comerciante : public Npc {
public:
    Comerciante(uint16_t id, Posicion posicion) : Npc(id, TipoNpc::Comerciante, posicion), itemsDisponibles({}) {}
    void agregarItemCatalogo(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta);

    std::map<uint16_t, std::pair<uint8_t, uint8_t>> listarItemsDisponibles();
    std::pair<bool, uint8_t> comprarItem(uint16_t idJugador, uint16_t idItem); // Devuelve <éxito, precioCompra>
    std::pair<bool, uint8_t> venderItem(uint16_t idJugador, uint16_t idItem);  // Devuelve <éxito, precioVenta>

private:
    std::map<uint16_t, std::pair<uint8_t, uint8_t>> itemsDisponibles; // <idItem, <precioCompra, precioVenta>>>
};

#endif