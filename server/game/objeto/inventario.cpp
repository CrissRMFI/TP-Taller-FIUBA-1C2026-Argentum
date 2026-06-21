#include "inventario.h"

#include <algorithm>
#include <cstddef>

static constexpr uint16_t ITEM_VACIO = 0;

static size_t contarItemsDesplazados(uint16_t primero, uint16_t segundo = ITEM_VACIO) {
    size_t cantidad = 0;
    if (primero != ITEM_VACIO) {
        cantidad++;
    }
    if (segundo != ITEM_VACIO) {
        cantidad++;
    }
    return cantidad;
}

Inventario::Inventario(uint8_t cantidadMaximaItems) : slots(cantidadMaximaItems, ITEM_VACIO) {}

bool Inventario::agregarItem(uint16_t idItem) {
    if (idItem == ITEM_VACIO) {
        return false;
    }

    for (uint16_t& slot : slots) {
        if (slot == ITEM_VACIO) {
            slot = idItem;
            return true;
        }
    }

    return false;
}

bool Inventario::eliminarItem(uint16_t idItem) {
    if (idItem == ITEM_VACIO) {
        return false;
    }

    for (uint16_t& slot : slots) {
        if (slot == idItem) {
            slot = ITEM_VACIO;
            return true;
        }
    }

    return false;
}

bool Inventario::eliminarSlot(uint8_t indice) {
    if (indice >= slots.size()) {
        return false;
    }

    if (slots[indice] == ITEM_VACIO) {
        return false;
    }

    slots[indice] = ITEM_VACIO;
    return true;
}

uint16_t Inventario::quitarDeSlot(uint8_t indice) {
    if (indice >= slots.size()) {
        return ITEM_VACIO;
    }

    uint16_t idItem = slots[indice];
    slots[indice] = ITEM_VACIO;
    return idItem;
}

void Inventario::agregarItemEnSlot(uint16_t idItem, uint8_t indice) {
    slots[indice] = idItem;
}

bool Inventario::tieneItem(uint16_t idItem) const {
    if (idItem == ITEM_VACIO) {
        return false;
    }

    for (uint16_t slot : slots) {
        if (slot == idItem) {
            return true;
        }
    }

    return false;
}

bool Inventario::tieneEspacioLibre() const {
    return cantidadSlotsLibres() > 0;
}

uint16_t Inventario::getIdEnSlot(uint8_t indice) const {
    if (indice >= slots.size()) {
        return ITEM_VACIO;
    }

    return slots[indice];
}

std::vector<uint16_t> Inventario::getSlots() const {
    return slots;
}

std::vector<uint16_t> Inventario::vaciar() {
    std::vector<uint16_t> items;

    for (uint16_t& slot : slots) {
        if (slot != ITEM_VACIO) {
            items.push_back(slot);
            slot = ITEM_VACIO;
        }
    }

    for (uint16_t item : equipamiento.getArma() != ITEM_VACIO
                                 ? std::vector<uint16_t>{equipamiento.getArma()}
                                 : std::vector<uint16_t>{}) {
        items.push_back(item);
        equipamiento.desequiparArma();
    }

    for (uint16_t item : equipamiento.getBaculo() != ITEM_VACIO
                                 ? std::vector<uint16_t>{equipamiento.getBaculo()}
                                 : std::vector<uint16_t>{}) {
        items.push_back(item);
        equipamiento.desequiparBaculo();
    }

    for (uint16_t item : equipamiento.getDefensa() != ITEM_VACIO
                                 ? std::vector<uint16_t>{equipamiento.getDefensa()}
                                 : std::vector<uint16_t>{}) {
        items.push_back(item);
        equipamiento.desequiparDefensa();
    }

    for (uint16_t item : equipamiento.getCasco() != ITEM_VACIO
                                 ? std::vector<uint16_t>{equipamiento.getCasco()}
                                 : std::vector<uint16_t>{}) {
        items.push_back(item);
        equipamiento.desequiparCasco();
    }

    for (uint16_t item : equipamiento.getEscudo() != ITEM_VACIO
                                 ? std::vector<uint16_t>{equipamiento.getEscudo()}
                                 : std::vector<uint16_t>{}) {
        items.push_back(item);
        equipamiento.desequiparEscudo();
    }

    return items;
}

size_t Inventario::cantidadSlotsLibres() const {
    size_t cantidad = 0;
    for (uint16_t slot : slots) {
        if (slot == ITEM_VACIO) {
            cantidad++;
        }
    }
    return cantidad;
}

bool Inventario::equiparItem(uint16_t idItem, TipoItem tipo) {
    if (idItem == ITEM_VACIO) {
        return false;
    }

    if (tipo == TipoItem::Defensa || tipo == TipoItem::Pocion) {
        return false;
    }

    if (!tieneItem(idItem)) {
        return false;
    }

    std::vector<uint16_t> desplazados;
    size_t espaciosLuegoDeQuitar = cantidadSlotsLibres() + 1;

    if (tipo == TipoItem::Arma) {
        if (contarItemsDesplazados(getArmaEquipada(), getBaculoEquipado()) >
            espaciosLuegoDeQuitar) {
            return false;
        }
        eliminarItem(idItem);
        desplazados = equipamiento.equiparArma(idItem);
    } else if (tipo == TipoItem::Baculo) {
        if (contarItemsDesplazados(getBaculoEquipado(), getArmaEquipada()) >
            espaciosLuegoDeQuitar) {
            return false;
        }
        eliminarItem(idItem);
        desplazados = equipamiento.equiparBaculo(idItem);
    } else {
        return false;
    }

    for (uint16_t desplazado : desplazados) {
        if (desplazado != ITEM_VACIO) {
            agregarItem(desplazado);
        }
    }

    return true;
}

bool Inventario::equiparPieza(uint16_t idItem, TipoDefensa slot) {
    if (idItem == ITEM_VACIO) {
        return false;
    }

    if (!tieneItem(idItem)) {
        return false;
    }

    uint16_t anterior = ITEM_VACIO;
    size_t espaciosLuegoDeQuitar = cantidadSlotsLibres() + 1;

    switch (slot) {
        case TipoDefensa::Armadura:
            if (contarItemsDesplazados(getDefensaEquipada()) > espaciosLuegoDeQuitar) {
                return false;
            }
            eliminarItem(idItem);
            anterior = equipamiento.equiparDefensa(idItem);
            break;

        case TipoDefensa::Casco:
            if (contarItemsDesplazados(getCascoEquipado()) > espaciosLuegoDeQuitar) {
                return false;
            }
            eliminarItem(idItem);
            anterior = equipamiento.equiparCasco(idItem);
            break;

        case TipoDefensa::Escudo:
            if (contarItemsDesplazados(getEscudoEquipado()) > espaciosLuegoDeQuitar) {
                return false;
            }
            eliminarItem(idItem);
            anterior = equipamiento.equiparEscudo(idItem);
            break;
    }

    if (anterior != ITEM_VACIO) {
        agregarItem(anterior);
    }

    return true;
}

bool Inventario::equiparSlot(uint8_t indice, TipoItem tipo) {
    if (tipo == TipoItem::Defensa || tipo == TipoItem::Pocion) {
        return false;
    }

    size_t espaciosLuegoDeQuitar = cantidadSlotsLibres() + 1;

    if (tipo == TipoItem::Arma &&
        contarItemsDesplazados(getArmaEquipada(), getBaculoEquipado()) > espaciosLuegoDeQuitar) {
        return false;
    }

    if (tipo == TipoItem::Baculo &&
        contarItemsDesplazados(getBaculoEquipado(), getArmaEquipada()) > espaciosLuegoDeQuitar) {
        return false;
    }

    uint16_t idItem = quitarDeSlot(indice);
    if (idItem == ITEM_VACIO) {
        return false;
    }

    std::vector<uint16_t> desplazados;

    if (tipo == TipoItem::Arma) {
        desplazados = equipamiento.equiparArma(idItem);
    } else if (tipo == TipoItem::Baculo) {
        desplazados = equipamiento.equiparBaculo(idItem);
    }

    for (uint16_t desplazado : desplazados) {
        if (desplazado != ITEM_VACIO) {
            agregarItem(desplazado);
        }
    }

    return true;
}

bool Inventario::equiparPiezaSlot(uint8_t indice, TipoDefensa slot) {
    size_t espaciosLuegoDeQuitar = cantidadSlotsLibres() + 1;

    switch (slot) {
        case TipoDefensa::Armadura:
            if (contarItemsDesplazados(getDefensaEquipada()) > espaciosLuegoDeQuitar) {
                return false;
            }
            break;

        case TipoDefensa::Casco:
            if (contarItemsDesplazados(getCascoEquipado()) > espaciosLuegoDeQuitar) {
                return false;
            }
            break;

        case TipoDefensa::Escudo:
            if (contarItemsDesplazados(getEscudoEquipado()) > espaciosLuegoDeQuitar) {
                return false;
            }
            break;
    }

    uint16_t idItem = quitarDeSlot(indice);
    if (idItem == ITEM_VACIO) {
        return false;
    }

    uint16_t anterior = ITEM_VACIO;

    switch (slot) {
        case TipoDefensa::Armadura:
            anterior = equipamiento.equiparDefensa(idItem);
            break;

        case TipoDefensa::Casco:
            anterior = equipamiento.equiparCasco(idItem);
            break;

        case TipoDefensa::Escudo:
            anterior = equipamiento.equiparEscudo(idItem);
            break;
    }

    if (anterior != ITEM_VACIO) {
        agregarItem(anterior);
    }

    return true;
}

bool Inventario::desequiparRanura(uint8_t ranura) {

    uint16_t idEquipado = ITEM_VACIO;
    switch (ranura) {
        case 0:
            idEquipado = getArmaEquipada();
            break;
        case 1:
            idEquipado = getBaculoEquipado();
            break;
        case 2:
            idEquipado = getDefensaEquipada();
            break;
        case 3:
            idEquipado = getCascoEquipado();
            break;
        case 4:
            idEquipado = getEscudoEquipado();
            break;
        default:
            return false;
    }

    if (idEquipado == ITEM_VACIO) {
        return false;
    }

    if (!tieneEspacioLibre()) {
        return false;
    }

    switch (ranura) {
        case 0:
            equipamiento.desequiparArma();
            break;
        case 1:
            equipamiento.desequiparBaculo();
            break;
        case 2:
            equipamiento.desequiparDefensa();
            break;
        case 3:
            equipamiento.desequiparCasco();
            break;
        case 4:
            equipamiento.desequiparEscudo();
            break;
        default:
            return false;
    }

    agregarItem(idEquipado);
    return true;
}

uint16_t Inventario::getArmaEquipada() const {
    return equipamiento.getArma();
}

uint16_t Inventario::getBaculoEquipado() const {
    return equipamiento.getBaculo();
}

uint16_t Inventario::getDefensaEquipada() const {
    return equipamiento.getDefensa();
}

uint16_t Inventario::getCascoEquipado() const {
    return equipamiento.getCasco();
}

uint16_t Inventario::getEscudoEquipado() const {
    return equipamiento.getEscudo();
}

void Inventario::restaurar(const std::vector<uint16_t>& slotsNuevos, uint16_t arma, uint16_t baculo,
                           uint16_t defensa, uint16_t casco, uint16_t escudo) {
    const size_t n = std::min(slotsNuevos.size(), slots.size());
    for (size_t i = 0; i < n; ++i) {
        slots[i] = slotsNuevos[i];
    }
    for (size_t i = n; i < slots.size(); ++i) {
        slots[i] = ITEM_VACIO;
    }
    equipamiento.equiparArma(arma);
    equipamiento.equiparBaculo(baculo);
    equipamiento.equiparDefensa(defensa);
    equipamiento.equiparCasco(casco);
    equipamiento.equiparEscudo(escudo);
}
