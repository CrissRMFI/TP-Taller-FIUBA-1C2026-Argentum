#include "mapa.h"
#include <array>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>

Mapa::Mapa(uint16_t ancho, uint16_t alto) : ancho(ancho), alto(alto) {
  if (ancho == 0 || alto == 0) {
    throw std::invalid_argument("Las dimensiones del mapa deben ser mayores a cero");
  }
}

size_t Mapa::cantidadCriaturas() const {
  return criaturas.size();
}

bool Mapa::mismaPosicion(const Posicion& primera, const Posicion& segunda) {
    return primera.mapaId == segunda.mapaId &&
           primera.x == segunda.x &&
           primera.y == segunda.y;
}

bool Mapa::agregarNpc(const Npc& npc) {
    if (!posicionValida(npc.getPosicion())) {
        return false;
    }

    if (hayParedEn(npc.getPosicion())) {
        return false;
    }

    if (hayCriaturaEn(npc.getPosicion())) {
        return false;
    }

    if (hayNpcEn(npc.getPosicion())) {
        return false;
    }

    const uint16_t id = npc.getId();

    switch (npc.getTipo()) {
      case TipoNpc::Comerciante: {
        auto resultado = comerciantes.emplace(id, Comerciante(id, npc.getPosicion()));
        if (!resultado.second) {
          return false;
        }
        return true;
      }
      case TipoNpc::Banquero: {
        auto resultado = banqueros.emplace(id, Banquero(id, npc.getPosicion()));
        if (!resultado.second) {
          return false;
        }
        return true;
      }
      case TipoNpc::Sacerdote: {
        auto resultado = sacerdotes.emplace(id, Sacerdote(id, npc.getPosicion()));
        if (!resultado.second) {
          return false;
        }
        return true;
      }
      default:
        return false;
    }
}

void Mapa::agregarStockComerciantes(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta) {
    for (auto& [id, comerciante] : comerciantes) {
        comerciante.agregarItemCatalogo(idItem, precioCompra, precioVenta);
    }
}

void Mapa::agregarStockSacerdotes(uint16_t idItem, uint8_t precio) {
    for (auto& [id, sacerdote] : sacerdotes) {
        sacerdote.agregarItemCatalogo(idItem, precio);
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
  std::optional<Npc> resultado;
  forEachNpc([&](const Npc& npc) {
    if (resultado.has_value()) {
      return;
    }
    const Posicion posicionNpc = npc.getPosicion();
    if (npc.getTipo() == tipo && posicion.mismaMapa(posicionNpc) && posicion.distanciaManhattan(posicionNpc) <= rango) {
      resultado = npc;
    }
  });
  return resultado;
}

std::optional<Npc> Mapa::buscarSacerdoteMasCercano(const Posicion& posicion) const {
  std::optional<Npc> masCercano;
  int distanciaMinima = 0;
  for (const auto& [id, sacerdote] : sacerdotes) {
    const Posicion posicionNpc = sacerdote.getPosicion();
    if (!posicion.mismaMapa(posicionNpc)) {
      continue;
    }
    const int distancia = posicion.distanciaManhattan(posicionNpc);
    if (!masCercano.has_value() || distancia < distanciaMinima) {
      masCercano = sacerdote;
      distanciaMinima = distancia;
    }
  }
  return masCercano;
}

std::optional<Posicion> Mapa::obtenerPosicionResurreccionCercana(const Posicion& origen) const {
    // BFS por anillos desde `origen`: devuelve la primera celda libre (sin pared,
    // NPC, criatura o ítem en el suelo) que se alcance. Itera en orden de
    // distancia Manhattan creciente, así la primera celda válida que se descole
    // es también la más cercana a `origen`.
    if (!posicionValida(origen)) {
        return std::nullopt;
    }

    std::queue<Posicion> cola;
    std::set<std::pair<uint16_t, uint16_t>> visitadas;

    cola.push(origen);
    visitadas.insert({origen.x, origen.y});

    static constexpr std::array<std::pair<int, int>, 4> direcciones = {{
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    }};

    while (!cola.empty()) {
        const Posicion actual = cola.front();
        cola.pop();

        if (!hayParedEn(actual) && !hayNpcEn(actual) &&
            !hayCriaturaEn(actual) && !hayItemEn(actual)) {
            return actual;
        }

        for (const auto& [dx, dy] : direcciones) {
            // Evitar underflow de uint16_t al restar 1 en los bordes.
            if ((dx < 0 && actual.x == 0) || (dy < 0 && actual.y == 0)) {
                continue;
            }
            const uint16_t nx = static_cast<uint16_t>(static_cast<int>(actual.x) + dx);
            const uint16_t ny = static_cast<uint16_t>(static_cast<int>(actual.y) + dy);

            // Bounds explícitos contra las dimensiones del mapa.
            if (nx >= ancho || ny >= alto) {
                continue;
            }

            if (visitadas.insert({nx, ny}).second) {
                cola.push(Posicion{nx, ny, origen.mapaId});
            }
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
    if (idItem == 0 || !posicionValida(posicion) || hayParedEn(posicion) ||
            hayNpcEn(posicion) || hayCriaturaEn(posicion) || hayItemEn(posicion)) {
        return false;
    }

    itemsEnSuelo.push_back({ idItem, posicion, 0.0f });
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
  std::optional<Npc> resultado;
  forEachNpc([&](const Npc& npc) {
    if (resultado.has_value()) {
      return;
    }
    if (mismaPosicion(npc.getPosicion(), posicion)) {
      resultado = npc;
    }
  });
  return resultado;
}

Sacerdote* Mapa::obtenerSacerdote(uint16_t idSacerdote) {
  auto it = sacerdotes.find(idSacerdote);
  return (it != sacerdotes.end()) ? &it->second : nullptr;
}

Comerciante* Mapa::obtenerComerciante(uint16_t idComerciante) {
  auto it = comerciantes.find(idComerciante);
  return (it != comerciantes.end()) ? &it->second : nullptr;
}

Banquero* Mapa::obtenerBanquero(uint16_t idBanquero) {
  auto it = banqueros.find(idBanquero);
  return (it != banqueros.end()) ? &it->second : nullptr;
}

const Sacerdote* Mapa::obtenerSacerdote(uint16_t idSacerdote) const {
  auto it = sacerdotes.find(idSacerdote);
  return (it != sacerdotes.end()) ? &it->second : nullptr;
}

const Comerciante* Mapa::obtenerComerciante(uint16_t idComerciante) const {
  auto it = comerciantes.find(idComerciante);
  return (it != comerciantes.end()) ? &it->second : nullptr;
}

const Banquero* Mapa::obtenerBanquero(uint16_t idBanquero) const {
  auto it = banqueros.find(idBanquero);
  return (it != banqueros.end()) ? &it->second : nullptr;
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

Criatura* Mapa::obtenerCriaturaPor(uint16_t idCriatura) {
  auto it = criaturas.find(idCriatura);
  return (it != criaturas.end()) ? &it->second : nullptr;
}

const Criatura* Mapa::obtenerCriaturaPor(uint16_t idCriatura) const {
  auto it = criaturas.find(idCriatura);
  return (it != criaturas.end()) ? &it->second : nullptr;
}

std::vector<Criatura> Mapa::obtenerCriaturas() const {
  std::vector<Criatura> resultado;
  for (const auto& [id, criatura] : criaturas) {
    resultado.push_back(criatura);
  }
  return resultado;
}

bool Mapa::agregarCriatura(const Criatura& criatura) {
    const Posicion posicion = criatura.getPos();

    if (!puedeOcuparCriatura(posicion)) {
        return false;
    }

    auto resultado = criaturas.emplace(criatura.getId(), criatura);

    if (!resultado.second) {
        return false;
    }

    return true;
}

bool Mapa::removerCriatura(uint16_t idCriatura) {
    auto it = criaturas.find(idCriatura);
    if (it == criaturas.end()) {
        return false;
    }
    criaturas.erase(it);
    return true;
}

bool Mapa::puedeOcuparCriatura(const Posicion& posicion) const {
  return posicionValida(posicion) && !esZonaSegura(posicion) && !hayParedEn(posicion) && !hayNpcEn(posicion) && !hayCriaturaEn(posicion);
}

bool Mapa::moverCriatura(uint16_t idCriatura, const Posicion& destino) {
    auto it = criaturas.find(idCriatura);

    if (it == criaturas.end()) {
        return false;
    }

    const Posicion origen = it->second.getPos();

    if (mismaPosicion(origen, destino)) {
        return true;
    }

    if (!puedeOcuparCriatura(destino)) {
        return false;
    }

    it->second.mover(destino);
    return true;
}

bool Mapa::hayOroEn(const Posicion& posicion) const {
    for (const OroEnSuelo& pila : orosEnSuelo) {
        if (mismaPosicion(pila.posicion, posicion)) {
            return true;
        }
    }
    return false;
}

bool Mapa::agregarOroEnSuelo(const Posicion& posicion, uint32_t cantidad) {
    if (cantidad == 0 || !posicionValida(posicion) || hayParedEn(posicion)) {
        return false;
    }

    // Si ya hay una pila en la misma celda, acumulamos (saturando en uint32_t).
    for (OroEnSuelo& pila : orosEnSuelo) {
        if (mismaPosicion(pila.posicion, posicion)) {
            const uint32_t restante = UINT32_MAX - pila.cantidad;
            pila.cantidad += std::min(cantidad, restante);
            pila.segundosEnSuelo = 0.0f;
            return true;
        }
    }

    orosEnSuelo.push_back({ cantidad, posicion, 0.0f });
    return true;
}

std::optional<uint32_t> Mapa::tomarOro(const Posicion& posicion) {
    if (!posicionValida(posicion) || hayParedEn(posicion)) {
        return std::nullopt;
    }

    for (auto it = orosEnSuelo.begin(); it != orosEnSuelo.end(); ++it) {
        if (mismaPosicion(it->posicion, posicion)) {
            uint32_t cantidad = it->cantidad;
            orosEnSuelo.erase(it);
            return cantidad;
        }
    }

    return std::nullopt;
}

std::vector<OroEnSuelo> Mapa::obtenerOroEnSuelo() const {
    return orosEnSuelo;
}

std::vector<OroEnSuelo> Mapa::actualizarOroEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg) {
    std::vector<OroEnSuelo> expiradas;
    std::vector<OroEnSuelo> vigentes;

    for (OroEnSuelo& pila : orosEnSuelo) {
        pila.segundosEnSuelo += deltaSegundos;
        if (pila.segundosEnSuelo >= tiempoMaximoSeg) {
            expiradas.push_back(pila);
        } else {
            vigentes.push_back(pila);
        }
    }

    orosEnSuelo = std::move(vigentes);
    return expiradas;
}

std::vector<ItemEnSuelo> Mapa::actualizarItemsEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg) {
    std::vector<ItemEnSuelo> itemsExpirados;
    std::vector<ItemEnSuelo> itemsVigentes;

    for (ItemEnSuelo& item : itemsEnSuelo) {
      item.segundosEnSuelo += deltaSegundos;
      if (item.segundosEnSuelo >= tiempoMaximoSeg) {
        itemsExpirados.push_back(item);
      } else {
        itemsVigentes.push_back(item);
      }

    }

    itemsEnSuelo = std::move(itemsVigentes);

    return itemsExpirados;
}
