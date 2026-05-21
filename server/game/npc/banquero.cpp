#include "banquero.h"


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

    cuentas[idJugador].first += cantidad;
    return true;
}

bool Banquero::retirarOro(uint16_t idJugador, uint32_t cantidad) {
    if (cuentas.find(idJugador) == cuentas.end() || cuentas[idJugador].first < cantidad) {
        return false;
    }

    cuentas[idJugador].first -= cantidad;
    return true;
}

bool Banquero::depositarItem(uint16_t idJugador, uint16_t idItem) {
    cuentas[idJugador].second.push_back(idItem);
    return true;
}

bool Banquero::retirarItem(uint16_t idJugador, uint16_t idItem) {
    if (cuentas.find(idJugador) == cuentas.end()) {
        return false;
    }

    auto& items = cuentas[idJugador].second;
    auto it = std::find(items.begin(), items.end(), idItem);
    if (it == items.end()) {
        return false;
    }

    items.erase(it);
    return true;
}