#ifndef SACERDOTE_H
#define SACERDOTE_H

#include "npc.h"

#include <map>

class Sacerdote : public Npc {
public:
    // Agregar factor de curación fijo en cfg
    Sacerdote(uint16_t id, Posicion posicion) : Npc(id, TipoNpc::Sacerdote, posicion), itemsDisponibles({}) {}
    void agregarItemCatalogo(uint16_t idItem, uint8_t precioVenta);

    std::pair<bool, uint8_t> venderItem(uint16_t idItem);  // Devuelve <éxito, precioVenta>
    std::map<uint16_t, uint8_t> listarItemsDisponibles();

private:
    std::map<uint16_t, uint8_t> itemsDisponibles; // <idItem, precioVenta>
};

#endif