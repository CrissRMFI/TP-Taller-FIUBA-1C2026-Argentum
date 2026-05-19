#include "mapa.h"
#include <stdexcept>

bool Mapa::mismaPosicion(const Posicion& primera, const Posicion& segunda) {
    return primera.mapaId == segunda.mapaId &&
           primera.x == segunda.x &&
           primera.y == segunda.y;
}

void Mapa::agregarNpc(const Npc& npc) {
    if (hayNpcEn(npc.getPosicion())) {
        throw std::invalid_argument("Ya existe un NPC en la misma posicion");
    }

    auto resultado = npcs.emplace(npc.getId(), npc);

    if (!resultado.second) {
        throw std::invalid_argument("Ya existe un NPC con el mismo id");
    }
}

bool Mapa::posicionValida(const Posicion& /*posicion*/) const {
    // TODO: validar dimensiones reales del mapa cuando existan tiles.
    return true;
}

bool Mapa::hayParedEn(const Posicion& /*posicion*/) const {
    // TODO: validar paredes reales cuando exista representación de tiles.
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
    if (!posicionValida(posicion) || hayParedEn(posicion) || hayNpcEn(posicion) || hayItemEn(posicion)) {
        return false;
    }
    
    itemsEnSuelo.push_back({ posicion, idItem });
    return true;
}

std::optional<uint16_t> Mapa::tomarItem(const Posicion& posicion) {
  for (auto it = itemsEnSuelo.begin(); it != itemsEnSuelo.end(); ++it) {
    if (mismaPosicion(it->posicion, posicion)) {
      uint16_t idItem = it->idItem;
      itemsEnSuelo.erase(it);
      return idItem;
    }
  }
  
  return std::nullopt;
}
