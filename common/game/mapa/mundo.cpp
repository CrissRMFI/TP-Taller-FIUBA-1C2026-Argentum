#include "mundo.h"

#include <utility>

Mundo::Mundo(Mapa&& mapaPrincipal, uint16_t mapaId) : principalId(mapaId) {
    mapas.emplace(mapaId, std::move(mapaPrincipal));
}

Mundo::Mundo(std::map<uint16_t, Mapa>&& mapas, std::vector<Portal>&& portales,
             uint16_t mapaPrincipalId) :
        mapas(std::move(mapas)),
        portales(std::move(portales)),
        principalId(mapaPrincipalId) {}

bool Mundo::existeMapa(uint16_t mapaId) const {
    return mapas.find(mapaId) != mapas.end();
}

std::optional<Posicion> Mundo::destinoPortalEn(const Posicion& origen) const {
    for (const Portal& portal : portales) {
        if (portal.origen == origen) {
            return portal.destino;
        }
    }
    return std::nullopt;
}

Mapa& Mundo::mapaDe(const Posicion& posicion) { return mapas.at(posicion.mapaId); }

const Mapa& Mundo::mapaDe(const Posicion& posicion) const { return mapas.at(posicion.mapaId); }

Mapa& Mundo::mapaDe(uint16_t mapaId) { return mapas.at(mapaId); }

const Mapa& Mundo::mapaDe(uint16_t mapaId) const { return mapas.at(mapaId); }

Mapa& Mundo::mapaPrincipal() { return mapas.at(principalId); }

const Mapa& Mundo::mapaPrincipal() const { return mapas.at(principalId); }

// ── Ruteadas por la posicion 

bool Mundo::posicionValida(const Posicion& posicion) const {
    return mapaDe(posicion).posicionValida(posicion);
}

bool Mundo::hayParedEn(const Posicion& posicion) const {
    return mapaDe(posicion).hayParedEn(posicion);
}

bool Mundo::hayObjetoEn(const Posicion& posicion) const {
    return mapaDe(posicion).hayObjetoEn(posicion);
}

bool Mundo::hayNpcEn(const Posicion& posicion) const {
    return mapaDe(posicion).hayNpcEn(posicion);
}

bool Mundo::hayCriaturaEn(const Posicion& posicion) const {
    return mapaDe(posicion).hayCriaturaEn(posicion);
}

bool Mundo::hayItemEn(const Posicion& posicion) const {
    return mapaDe(posicion).hayItemEn(posicion);
}

bool Mundo::hayOroEn(const Posicion& posicion) const {
    return mapaDe(posicion).hayOroEn(posicion);
}

bool Mundo::esCiudad(const Posicion& posicion) const {
    return mapaDe(posicion).esCiudad(posicion);
}

bool Mundo::esZonaSegura(const Posicion& posicion) const {
    return mapaDe(posicion).esZonaSegura(posicion);
}

bool Mundo::puedeOcuparCriatura(const Posicion& posicion) const {
    return mapaDe(posicion).puedeOcuparCriatura(posicion);
}

bool Mundo::agregarItem(const Posicion& posicion, uint16_t idItem) {
    return mapaDe(posicion).agregarItem(posicion, idItem);
}

std::optional<uint16_t> Mundo::tomarItem(const Posicion& posicion) {
    return mapaDe(posicion).tomarItem(posicion);
}

std::optional<uint32_t> Mundo::tomarOro(const Posicion& posicion) {
    return mapaDe(posicion).tomarOro(posicion);
}

bool Mundo::agregarOroEnSuelo(const Posicion& posicion, uint32_t cantidad) {
    return mapaDe(posicion).agregarOroEnSuelo(posicion, cantidad);
}

std::optional<Npc> Mundo::buscarSacerdoteMasCercano(const Posicion& posicion) const {
    return mapaDe(posicion).buscarSacerdoteMasCercano(posicion);
}

std::optional<Posicion> Mundo::buscarCeldaLibreCercaDe(
        const Posicion& origen,
        const std::function<bool(const Posicion&)>& celdaOcupada) const {
    return mapaDe(origen).buscarCeldaLibreCercaDe(origen, celdaOcupada);
}

// ── Criaturas ───────────────────────────────────────────────────────────────

bool Mundo::agregarCriatura(const Criatura& criatura) {
    return mapaDe(criatura.getPos()).agregarCriatura(criatura);
}

bool Mundo::removerCriatura(uint16_t idCriatura) {
    for (auto& [id, mapa] : mapas) {
        if (mapa.removerCriatura(idCriatura)) {
            return true;
        }
    }
    return false;
}

bool Mundo::moverCriatura(uint16_t idCriatura, const Posicion& destino) {
    return mapaDe(destino).moverCriatura(idCriatura, destino);
}

Criatura* Mundo::obtenerCriaturaPor(uint16_t idCriatura) {
    for (auto& [id, mapa] : mapas) {
        if (Criatura* criatura = mapa.obtenerCriaturaPor(idCriatura)) {
            return criatura;
        }
    }
    return nullptr;
}

const Criatura* Mundo::obtenerCriaturaPor(uint16_t idCriatura) const {
    for (const auto& [id, mapa] : mapas) {
        if (const Criatura* criatura = mapa.obtenerCriaturaPor(idCriatura)) {
            return criatura;
        }
    }
    return nullptr;
}

// Sacerdote por id

Sacerdote* Mundo::obtenerSacerdote(uint16_t idSacerdote) {
    for (auto& [id, mapa] : mapas) {
        if (Sacerdote* sacerdote = mapa.obtenerSacerdote(idSacerdote)) {
            return sacerdote;
        }
    }
    return nullptr;
}

const Sacerdote* Mundo::obtenerSacerdote(uint16_t idSacerdote) const {
    for (const auto& [id, mapa] : mapas) {
        if (const Sacerdote* sacerdote = mapa.obtenerSacerdote(idSacerdote)) {
            return sacerdote;
        }
    }
    return nullptr;
}

// Agregados sobre todos los mapas

std::vector<Criatura> Mundo::obtenerCriaturas() const {
    std::vector<Criatura> todas;
    for (const auto& [id, mapa] : mapas) {
        std::vector<Criatura> criaturasMapa = mapa.obtenerCriaturas();
        todas.insert(todas.end(), criaturasMapa.begin(), criaturasMapa.end());
    }
    return todas;
}

std::vector<ItemEnSuelo> Mundo::obtenerItemsEnSuelo() const {
    std::vector<ItemEnSuelo> todos;
    for (const auto& [id, mapa] : mapas) {
        std::vector<ItemEnSuelo> itemsMapa = mapa.obtenerItemsEnSuelo();
        todos.insert(todos.end(), itemsMapa.begin(), itemsMapa.end());
    }
    return todos;
}

std::vector<OroEnSuelo> Mundo::obtenerOroEnSuelo() const {
    std::vector<OroEnSuelo> todos;
    for (const auto& [id, mapa] : mapas) {
        std::vector<OroEnSuelo> orosMapa = mapa.obtenerOroEnSuelo();
        todos.insert(todos.end(), orosMapa.begin(), orosMapa.end());
    }
    return todos;
}

size_t Mundo::cantidadCriaturas() const {
    size_t total = 0;
    for (const auto& [id, mapa] : mapas) {
        total += mapa.cantidadCriaturas();
    }
    return total;
}

size_t Mundo::cantidadCriaturasEn(uint16_t mapaId) const {
    const auto it = mapas.find(mapaId);
    return (it != mapas.end()) ? it->second.cantidadCriaturas() : 0;
}

uint16_t Mundo::mapaPrincipalId() const {
    return principalId;
}

std::vector<ItemEnSuelo> Mundo::actualizarItemsEnSuelo(float deltaSegundos,
                                                       uint16_t tiempoMaximoSeg) {
    std::vector<ItemEnSuelo> expirados;
    for (auto& [id, mapa] : mapas) {
        std::vector<ItemEnSuelo> expiradosMapa =
                mapa.actualizarItemsEnSuelo(deltaSegundos, tiempoMaximoSeg);
        expirados.insert(expirados.end(), expiradosMapa.begin(), expiradosMapa.end());
    }
    return expirados;
}

std::vector<OroEnSuelo> Mundo::actualizarOroEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg) {
    std::vector<OroEnSuelo> expirados;
    for (auto& [id, mapa] : mapas) {
        std::vector<OroEnSuelo> expiradosMapa =
                mapa.actualizarOroEnSuelo(deltaSegundos, tiempoMaximoSeg);
        expirados.insert(expirados.end(), expiradosMapa.begin(), expiradosMapa.end());
    }
    return expirados;
}

void Mundo::agregarStockComerciantes(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta) {
    for (auto& [id, mapa] : mapas) {
        mapa.agregarStockComerciantes(idItem, precioCompra, precioVenta);
    }
}

void Mundo::agregarStockSacerdotes(uint16_t idItem, uint8_t precio) {
    for (auto& [id, mapa] : mapas) {
        mapa.agregarStockSacerdotes(idItem, precio);
    }
}

const std::map<uint16_t, Sacerdote>& Mundo::getSacerdotes() const {
    return mapaPrincipal().getSacerdotes();
}

const std::map<uint16_t, Comerciante>& Mundo::getComerciantes() const {
    return mapaPrincipal().getComerciantes();
}

const std::map<uint16_t, Banquero>& Mundo::getBanqueros() const {
    return mapaPrincipal().getBanqueros();
}
