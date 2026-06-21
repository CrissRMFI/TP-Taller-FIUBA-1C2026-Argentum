#ifndef COMERCIANTE_H
#define COMERCIANTE_H

#include "npc.h"

#include <map>
#include <vector>

class Comerciante : public Npc {
public:
    Comerciante(uint16_t id, Posicion posicion);
    void agregarItemCatalogo(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta);

    std::map<uint16_t, std::pair<uint8_t, uint8_t>> listarItemsDisponibles();
    std::pair<bool, uint8_t> comprarItem(uint16_t idItem);
    std::pair<bool, uint8_t> venderItem(uint16_t idItem);

private:
    std::map<uint16_t, std::pair<uint8_t, uint8_t>> itemsDisponibles;
};

#endif