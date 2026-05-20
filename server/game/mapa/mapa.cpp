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

    if (hayCriaturaEn(npc.getPosicion())) {
        throw std::invalid_argument("No se puede agregar un NPC sobre una criatura");
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

    if (hayCriaturaEn(posicion)) {
      throw std::invalid_argument("No se puede agregar una pared sobre una criatura");
    }

    paredes.push_back(posicion);
}

void Mapa::agregarCiudad(const Ciudad& ciudad) {
    if (ciudad.xMin > ciudad.xMax || ciudad.yMin > ciudad.yMax) {
        throw std::invalid_argument("La ciudad tiene limites invalidos");
    }

    if (ciudad.xMax >= ancho || ciudad.yMax >= alto) {
        throw std::invalid_argument("La ciudad esta fuera de los limites del mapa");
    }

    ciudades.push_back(ciudad);
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

std::optional<Npc> Mapa::buscarNpcCercano(const Posicion& posicion, TipoNpc tipo, uint16_t rango) const {
  
  for (const auto& [id, npc] : npcs) {
    const Posicion posicionNpc = npc.getPosicion();
    
    if (npc.getTipo() == tipo && posicion.mismaMapa(posicionNpc) && posicion.distanciaManhattan(posicionNpc) <= rango) {
      return npc;
    }
  }
  return std::nullopt;
}

bool Mapa::hayNpcCercano(const Posicion& posicion, TipoNpc tipo, uint16_t rango) const {
  return buscarNpcCercano(posicion, tipo, rango).has_value();
}

bool Mapa::hayNpcEn(const Posicion& posicion) const {
  return buscarNpcEn(posicion).has_value();
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

bool Mapa::esCiudad(const Posicion& posicion) const {
    for (const Ciudad& ciudad : ciudades) {
        if (ciudad.mapaId == posicion.mapaId && posicion.x >= ciudad.xMin && posicion.x <= ciudad.xMax && posicion.y >= ciudad.yMin && posicion.y <= ciudad.yMax) {
            return true;
        }
    }

    return false;
}

bool Mapa::esZonaSegura(const Posicion& posicion) const {
    return esCiudad(posicion);
}

std::optional<Npc> Mapa::buscarNpcEn(const Posicion& posicion) const {
  for (const auto& [id, npc] : npcs) {
    if (mismaPosicion(npc.getPosicion(), posicion)) {
      return npc;
    }
  }
  return std::nullopt;
}

std::vector<Npc> Mapa::obtenerNpcs() const {
  std::vector<Npc> resultado;
  for (const auto& [id, npc] : npcs) {
    resultado.push_back(npc);
  }
  return resultado;
}

std::vector<Npc> Mapa::obtenerNpcsPorTipo(TipoNpc tipo) const {
  std::vector<Npc> resultado;
  
  for (const auto& [id, npc] : npcs) {
    if (npc.getTipo() == tipo) {
      resultado.push_back(npc);
    }
  }
  return resultado;
}

std::optional<Criatura> Mapa::buscarCriaturaEn(const Posicion& posicion) const {
  for (const auto& [id, criatura] : criaturas) {
    if (mismaPosicion(criatura.getPos(), posicion)) {
      return criatura;
    }
  }
  
  return std::nullopt;
}

bool Mapa::hayCriaturaEn(const Posicion& posicion) const {
  return buscarCriaturaEn(posicion).has_value();
}

std::vector<Criatura> Mapa::obtenerCriaturas() const {
  std::vector<Criatura> resultado;
  for (const auto& [id, criatura] : criaturas) {
    resultado.push_back(criatura);
  }
  return resultado;
}

void Mapa::agregarCriatura(const Criatura& criatura) {
    const Posicion posicion = criatura.getPos();

    if (!puedeOcuparCriatura(posicion)) {
        throw std::invalid_argument("La criatura no puede ocupar esa posicion");
    }

    auto resultado = criaturas.emplace(criatura.getId(), criatura);

    if (!resultado.second) {
        throw std::invalid_argument("Ya existe una criatura con el mismo id");
    }
}

bool Mapa::puedeOcuparCriatura(const Posicion& posicion) const {
  return posicionValida(posicion) && !esZonaSegura(posicion) && !hayParedEn(posicion) && !hayNpcEn(posicion) && !hayCriaturaEn(posicion);
}

void Mapa::moverCriatura(uint16_t idCriatura, const Posicion& destino) {
    auto it = criaturas.find(idCriatura);

    if (it == criaturas.end()) {
        throw std::invalid_argument("No existe una criatura con ese id");
    }

    const Posicion origen = it->second.getPos();

    if (mismaPosicion(origen, destino)) {
        return;
    }

    if (!puedeOcuparCriatura(destino)) {
        throw std::invalid_argument("La criatura no puede moverse a esa posicion");
    }

    it->second.mover(destino);
}
