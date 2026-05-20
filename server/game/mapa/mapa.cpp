#include "mapa.h"
#include <stdexcept>

Mapa::Mapa(uint16_t ancho, uint16_t alto) : ancho(ancho), alto(alto) {
  if (ancho == 0 || alto == 0) {
    throw std::invalid_argument("Las dimensiones del mapa deben ser mayores a cero");
  }
}

bool Mapa::mismaPosicion(const Posicion& primera, const Posicion& segunda) {
    return primera.mapaId == segunda.mapaId &&
           primera.x == segunda.x &&
           primera.y == segunda.y;
}

void Mapa::agregarNpc(const Npc& npc) {
  
  if (!posicionValida(npc.getPosicion())) {
    throw std::invalid_argument("El NPC esta fuera de los limites del mapa");
  }
  
  if (hayParedEn(npc.getPosicion())) {
    throw std::invalid_argument("No se puede agregar un NPC sobre una pared");
  }
  
  if (hayNpcEn(npc.getPosicion())) {
    throw std::invalid_argument("Ya existe un NPC en la misma posicion");
  }
  
  auto resultado = npcs.emplace(npc.getId(), npc);
  
  if (!resultado.second) {
    throw std::invalid_argument("Ya existe un NPC con el mismo id");
  }
}

void Mapa::agregarPared(const Posicion& posicion) {
    if (!posicionValida(posicion)) {
        throw std::invalid_argument("La pared esta fuera de los limites del mapa");
    }

    if (hayParedEn(posicion)) {
        throw std::invalid_argument("Ya existe una pared en esa posicion");
    }

    if (hayNpcEn(posicion)) {
        throw std::invalid_argument("No se puede agregar una pared sobre un NPC");
    }

    paredes.push_back(posicion);
}

bool Mapa::posicionValida(const Posicion& posicion) const {
  return posicion.x < ancho && posicion.y < alto;
}

bool Mapa::hayParedEn(const Posicion& posicion) const {
  for (const Posicion& pared : paredes) {
    if (mismaPosicion(pared, posicion)) {
      return true;
    }
  }
  return false;
}

std::optional<Npc> Mapa::buscarNpcCercano(Posicion posicion, TipoNpc tipo) const {
    for (const auto& [id, npc]: npcs) {
        if (npc.getTipo() == tipo && mismaPosicion(npc.getPosicion(), posicion)) {
            return npc;
        }
    }

    return std::nullopt;
}

bool Mapa::hayNpcCercano(Posicion posicion, TipoNpc tipo) const {
    return buscarNpcCercano(posicion, tipo).has_value();
}

bool Mapa::hayNpcEn(const Posicion& posicion) const {
  for (const auto& [id, npc]: npcs) {
    if (mismaPosicion(npc.getPosicion(), posicion)) {
      return true;
    }
  }
  return false;
}

bool Mapa::hayItemEn(const Posicion& posicion) const {
  for (const ItemEnSuelo& item: itemsEnSuelo) {
    if (mismaPosicion(item.posicion, posicion)) {
      return true;
    }
  }
  
  return false;
}

bool Mapa::agregarItem(const Posicion& posicion, uint16_t idItem) {
    if ( idItem == 0 ||!posicionValida(posicion) || hayParedEn(posicion) || hayNpcEn(posicion) || hayItemEn(posicion)) {
        return false;
    }
    
    itemsEnSuelo.push_back({ posicion, idItem });
    return true;
}

std::optional<uint16_t> Mapa::tomarItem(const Posicion& posicion) {
    if (!posicionValida(posicion) || hayParedEn(posicion)) {
        return std::nullopt;
    }

    for (auto it = itemsEnSuelo.begin(); it != itemsEnSuelo.end(); ++it) {
        if (mismaPosicion(it->posicion, posicion)) {
            uint16_t idItem = it->idItem;
            itemsEnSuelo.erase(it);
            return idItem;
        }
    }

    return std::nullopt;
}

std::vector<ItemEnSuelo> Mapa::obtenerItemsEnSuelo() const {
    return itemsEnSuelo;
}
