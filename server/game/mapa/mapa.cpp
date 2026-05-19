#include "mapa.h"
#include <stdexcept>

bool Mapa::mismaPosicion(const Posicion& primera, const Posicion& segunda) {
    return primera.mapaId == segunda.mapaId &&
           primera.x == segunda.x &&
           primera.y == segunda.y;
}

void Mapa::agregarNpc(const Npc& npc) {
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
