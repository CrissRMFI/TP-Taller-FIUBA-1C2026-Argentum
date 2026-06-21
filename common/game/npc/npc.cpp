#include "npc.h"

Npc::Npc(uint16_t id, TipoNpc tipo, Posicion posicion) : id(id), tipo(tipo), posicion(posicion) {}

uint16_t Npc::getId() const {
    return id;
}

TipoNpc Npc::getTipo() const {
    return tipo;
}

Posicion Npc::getPosicion() const {
    return posicion;
}
