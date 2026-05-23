#include "sacerdote.h"

void Sacerdote::agregarItemCatalogo(uint16_t idItem, uint8_t precioVenta) {
    itemsDisponibles[idItem] = precioVenta;
}

std::pair<bool, uint8_t> Sacerdote::venderItem(uint16_t idItem) {
    if (itemsDisponibles.find(idItem) == itemsDisponibles.end()) {
        return {false, 0};
    }

    uint8_t precioVenta = itemsDisponibles[idItem];
    return {true, precioVenta};
}

std::map<uint16_t, uint8_t> Sacerdote::listarItemsDisponibles() {
    return itemsDisponibles;
}
