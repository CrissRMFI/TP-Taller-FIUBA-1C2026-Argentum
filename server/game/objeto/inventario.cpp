#include "inventario.h"

#include <algorithm>

Inventario::Inventario() {
    std::fill(slots, slots + cantidadMaximaItems, uint16_t(0));
}

bool Inventario::agregarItem(uint16_t idItem) {
    for (int i = 0; i < cantidadMaximaItems; i++) {
        if (slots[i] == 0) {
            slots[i] = idItem;
            return true;
        }
    }
    return false;
}

bool Inventario::eliminarItem(uint16_t idItem) {
    for (int i = 0; i < cantidadMaximaItems; i++) {
        if (slots[i] == idItem) {
            slots[i] = 0;
            return true;
        }
    }
    return false;
}

bool Inventario::tieneItem(uint16_t idItem) const {
    for (int i = 0; i < cantidadMaximaItems; i++)
        if (slots[i] == idItem) return true;
    return false;
}

uint16_t Inventario::getIdEnSlot(uint8_t indice) const {
    if (indice >= cantidadMaximaItems) return 0;
    return slots[indice];
}

std::vector<uint16_t> Inventario::vaciar() {
    std::vector<uint16_t> dropped;
    for (int i = 0; i < cantidadMaximaItems; i++) {
        if (slots[i] != 0) {
            dropped.push_back(slots[i]);
            slots[i] = 0;
        }
    }
    return dropped;
}

bool Inventario::equiparItem(uint16_t idItem, TipoItem tipo) {
    if (tipo == TipoItem::Defensa || tipo == TipoItem::Pocion) return false;
    if (!tieneItem(idItem)) return false;
    eliminarItem(idItem);

    std::vector<uint16_t> desplazados;
    if (tipo == TipoItem::Arma)
        desplazados = equipamiento.equiparArma(idItem);
    else
        desplazados = equipamiento.equiparBaculo(idItem);

    for (uint16_t id : desplazados)
        if (id != 0) agregarItem(id);
    return true;
}

bool Inventario::equiparPieza(uint16_t idItem, TipoDefensa slot) {
    if (!tieneItem(idItem)) return false;
    eliminarItem(idItem);

    uint16_t anterior = 0;
    switch (slot) {
        case TipoDefensa::Armadura: anterior = equipamiento.equiparDefensa(idItem); break;
        case TipoDefensa::Casco:    anterior = equipamiento.equiparCasco(idItem);   break;
        case TipoDefensa::Escudo:   anterior = equipamiento.equiparEscudo(idItem);  break;
    }
    if (anterior != 0) agregarItem(anterior);
    return true;
}

uint16_t Inventario::getArmaEquipada()    const { return equipamiento.getArma();    }
uint16_t Inventario::getBaculoEquipado()  const { return equipamiento.getBaculo();  }
uint16_t Inventario::getDefensaEquipada() const { return equipamiento.getDefensa(); }
uint16_t Inventario::getCascoEquipado()   const { return equipamiento.getCasco();   }
uint16_t Inventario::getEscudoEquipado()  const { return equipamiento.getEscudo();  }
