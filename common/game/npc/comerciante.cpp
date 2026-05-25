#include "comerciante.h"

void Comerciante::agregarItemCatalogo(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta) {
    itemsDisponibles[idItem] = {precioCompra, precioVenta};
}

std::map<uint16_t, std::pair<uint8_t, uint8_t>> Comerciante::listarItemsDisponibles() {
    return itemsDisponibles;
}

std::pair<bool, uint8_t> Comerciante::comprarItem(uint16_t idItem) {
    if (itemsDisponibles.find(idItem) == itemsDisponibles.end()) {
        return {false, 0};
    }

    uint8_t precioCompra = itemsDisponibles[idItem].first;
    return {true, precioCompra};
}

std::pair<bool, uint8_t> Comerciante::venderItem(uint16_t idItem) {
    if (itemsDisponibles.find(idItem) == itemsDisponibles.end()) {
        return {false, 0};
    }

    uint8_t precioVenta = itemsDisponibles[idItem].second;
    return {true, precioVenta};
}
