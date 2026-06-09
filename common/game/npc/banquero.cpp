#include "banquero.h"

#include <limits>

Banquero::Banquero(uint16_t id, Posicion posicion) : 
  Npc(id, TipoNpc::Banquero, posicion) 
  {cuentas = {};
}

std::pair<uint32_t, std::vector<uint16_t>> Banquero::listarItemsDisponibles(uint16_t idJugador) {
    if (cuentas.find(idJugador) != cuentas.end()) {
        return cuentas[idJugador];
    }
    return {0, {}};
}

bool Banquero::depositarOro(uint16_t idJugador, uint32_t cantidad) {
    if (cantidad == 0) {
        return false;
    }

    uint32_t& oroDepositado = cuentas[idJugador].first;
    if (cantidad > std::numeric_limits<uint32_t>::max() - oroDepositado) {
        return false;
    }

    oroDepositado += cantidad;
    return true;
}

bool Banquero::retirarOro(uint16_t idJugador, uint32_t cantidad) {
    if (cantidad == 0) {
        return false;
    }

    auto itCuenta = cuentas.find(idJugador);
    if (itCuenta == cuentas.end() || itCuenta->second.first < cantidad) {
        return false;
    }

    itCuenta->second.first -= cantidad;
    return true;
}

bool Banquero::depositarItem(uint16_t idJugador, uint16_t idItem) {
    
    if (idItem == 0) {
        return false;
    }

    cuentas[idJugador].second.push_back(idItem);
    return true;
}

bool Banquero::retirarItem(uint16_t idJugador, uint16_t idItem) {
    auto itCuenta = cuentas.find(idJugador);
    if (itCuenta == cuentas.end()) {
        return false;
    }

    auto& itemsDeLaCuenta = itCuenta->second.second;
    auto itItem = std::find(itemsDeLaCuenta.begin(), itemsDeLaCuenta.end(), idItem);
    if (itItem == itemsDeLaCuenta.end()) {
        return false;
    }

    itemsDeLaCuenta.erase(itItem);
    return true;
}

bool Banquero::tieneItem(uint16_t idJugador, uint16_t idItem) const {
    auto itCuenta = cuentas.find(idJugador);
    if (itCuenta == cuentas.end()) {
        return false;
    }

    const auto& itemsDeLaCuenta = itCuenta->second.second;
    return std::find(itemsDeLaCuenta.begin(), itemsDeLaCuenta.end(), idItem) != itemsDeLaCuenta.end();
}
