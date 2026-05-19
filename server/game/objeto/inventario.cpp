#include "inventario.h"

#include <cstddef>

static constexpr uint16_t ITEM_VACIO = 0;

Inventario::Inventario(uint8_t cantidadMaximaItems) :
        slots(cantidadMaximaItems, ITEM_VACIO) {}

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

    return items;
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

    eliminarItem(idItem);

    std::vector<uint16_t> desplazados;

    if (tipo == TipoItem::Arma) {
        desplazados = equipamiento.equiparArma(idItem);
    } else if (tipo == TipoItem::Baculo) {
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

    eliminarItem(idItem);

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

bool Inventario::equiparSlot(uint8_t indice, TipoItem tipo) {
    uint16_t idItem = quitarDeSlot(indice);
    if (idItem == ITEM_VACIO) {
        return false;
    }

    std::vector<uint16_t> desplazados;

    if (tipo == TipoItem::Arma) {
        desplazados = equipamiento.equiparArma(idItem);
    } else if (tipo == TipoItem::Baculo) {
        desplazados = equipamiento.equiparBaculo(idItem);
    } else {
        agregarItem(idItem);
        return false;
    }

    for (uint16_t desplazado : desplazados) {
        if (desplazado != ITEM_VACIO) {
            agregarItem(desplazado);
        }
    }

    return true;
}

bool Inventario::equiparPiezaSlot(uint8_t indice, TipoDefensa slot) {
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
