#include "comerciante.h"

Comerciante::Comerciante(uint16_t id, Posicion posicion) : Npc(id, TipoNpc::Comerciante, posicion), itemsDisponibles({}) {}

void Comerciante::agregarItemCatalogo(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta)
{
    itemsDisponibles[idItem] = {precioCompra, precioVenta};
}

std::map<uint16_t, std::pair<uint8_t, uint8_t>> Comerciante::listarItemsDisponibles() {
    return itemsDisponibles;
}

// El comerciante le COMPRA al jugador (el jugador vende): el jugador recibe el precio de venta (el menor). itemsDisponibles guarda {precioCompraJugador, precioVentaJugador}.
std::pair<bool, uint8_t> Comerciante::comprarItem(uint16_t idItem) {
    if (itemsDisponibles.find(idItem) == itemsDisponibles.end()) {
        return {false, 0};
    }

    uint8_t precioVentaJugador = itemsDisponibles[idItem].second;
    return {true, precioVentaJugador};
}

// El comerciante le VENDE al jugador (el jugador compra): el jugador paga el precio de compra (el mayor).
std::pair<bool, uint8_t> Comerciante::venderItem(uint16_t idItem) {
    if (itemsDisponibles.find(idItem) == itemsDisponibles.end()) {
        return {false, 0};
    }

    uint8_t precioCompraJugador = itemsDisponibles[idItem].first;
    return {true, precioCompraJugador};
}
