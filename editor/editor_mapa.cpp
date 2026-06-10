#include "editor_mapa.h"

#include <algorithm>

EditorMapa::EditorMapa(uint16_t ancho, uint16_t alto):
        ancho(ancho), alto(alto), mapaId(0) {}

uint16_t EditorMapa::getAncho() const { return ancho; }
uint16_t EditorMapa::getAlto() const { return alto; }
uint16_t EditorMapa::getMapaId() const { return mapaId; }
void EditorMapa::setMapaId(uint16_t id) { mapaId = id; }

bool EditorMapa::dentroDeLimites(uint16_t x, uint16_t y) const {
    return x < ancho && y < alto;
}

bool EditorMapa::hayParedEn(uint16_t x, uint16_t y) const {
    for (const Posicion& p : paredes) {
        if (p.x == x && p.y == y) {
            return true;
        }
    }
    return false;
}

bool EditorMapa::hayNpcEn(uint16_t x, uint16_t y) const {
    for (const NpcEditor& n : npcs) {
        if (n.x == x && n.y == y) {
            return true;
        }
    }
    return false;
}

bool EditorMapa::hayCriaturaEn(uint16_t x, uint16_t y) const {
    for (const CriaturaEditor& c : criaturas) {
        if (c.x == x && c.y == y) {
            return true;
        }
    }
    return false;
}

bool EditorMapa::estaEnCiudad(uint16_t x, uint16_t y) const {
    for (const Ciudad& c : ciudades) {
        if (x >= c.xMin && x <= c.xMax && y >= c.yMin && y <= c.yMax) {
            return true;
        }
    }
    return false;
}

bool EditorMapa::celdaOcupada(uint16_t x, uint16_t y) const {
    return hayParedEn(x, y) || hayNpcEn(x, y) || hayCriaturaEn(x, y);
}

void EditorMapa::ponerPared(uint16_t x, uint16_t y) {
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y)) {
        return;
    }
    paredes.push_back(Posicion{x, y, mapaId});
}

void EditorMapa::ponerNpc(TipoNpc tipo, uint16_t x, uint16_t y) {
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y)) {
        return;
    }
    npcs.push_back(NpcEditor{proximoIdNpc(), tipo, x, y});
}

void EditorMapa::ponerCriatura(TipoCriatura tipo, uint16_t x, uint16_t y) {
    // Las criaturas no pueden ir en zona segura (ciudad): el modelo del juego
    // las rechaza, asi que tampoco las dejamos colocar aca para que el guardado
    // y la recarga sean consistentes.
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y) || estaEnCiudad(x, y)) {
        return;
    }
    criaturas.push_back(CriaturaEditor{proximoIdCriatura(), tipo, x, y});
}

void EditorMapa::agregarCiudad(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    const uint16_t xMin = std::min(x1, x2);
    const uint16_t yMin = std::min(y1, y2);
    const uint16_t xMax = std::max(x1, x2);
    const uint16_t yMax = std::max(y1, y2);
    ciudades.push_back(Ciudad{mapaId, xMin, yMin, xMax, yMax});
}

void EditorMapa::pintarParedes(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    const uint16_t xMin = std::min(x1, x2);
    const uint16_t yMin = std::min(y1, y2);
    const uint16_t xMax = std::max(x1, x2);
    const uint16_t yMax = std::max(y1, y2);
    for (uint16_t y = yMin; y <= yMax; ++y) {
        for (uint16_t x = xMin; x <= xMax; ++x) {
            if (dentroDeLimites(x, y) && !celdaOcupada(x, y)) {
                paredes.push_back(Posicion{x, y, mapaId});
            }
        }
    }
}

void EditorMapa::pintarPiso(const std::string& clave, uint16_t x1, uint16_t y1,
                            uint16_t x2, uint16_t y2) {
    const uint16_t xMin = std::min(x1, x2);
    const uint16_t yMin = std::min(y1, y2);
    const uint16_t xMax = std::max(x1, x2);
    const uint16_t yMax = std::max(y1, y2);
    pisos.push_back(ZonaPiso{mapaId, xMin, yMin, xMax, yMax, clave});
}

std::string EditorMapa::pisoEn(uint16_t x, uint16_t y) const {
    for (auto it = pisos.rbegin(); it != pisos.rend(); ++it) {
        if (x >= it->xMin && x <= it->xMax && y >= it->yMin && y <= it->yMax) {
            return it->clave;
        }
    }
    return "pasto";
}

bool EditorMapa::todoCubierto() const {
    for (uint16_t y = 0; y < alto; ++y) {
        for (uint16_t x = 0; x < ancho; ++x) {
            if (pisoEn(x, y).empty()) {
                return false;
            }
        }
    }
    return true;
}

void EditorMapa::borrarEn(uint16_t x, uint16_t y) {
    // Goma "de arriba hacia abajo": saca lo primero que encuentre en la celda.
    // 1) Entidades (criatura / npc).
    for (std::vector<CriaturaEditor>::iterator it = criaturas.begin(); it != criaturas.end(); ++it) {
        if (it->x == x && it->y == y) {
            criaturas.erase(it);
            return;
        }
    }
    for (std::vector<NpcEditor>::iterator it = npcs.begin(); it != npcs.end(); ++it) {
        if (it->x == x && it->y == y) {
            npcs.erase(it);
            return;
        }
    }
    // 2) Pared.
    for (std::vector<Posicion>::iterator it = paredes.begin(); it != paredes.end(); ++it) {
        if (it->x == x && it->y == y) {
            paredes.erase(it);
            return;
        }
    }
    // 3) Zona de piso que cubre la celda (la de mas arriba; revierte a pasto u otra).
    for (std::vector<ZonaPiso>::reverse_iterator it = pisos.rbegin(); it != pisos.rend(); ++it) {
        if (x >= it->xMin && x <= it->xMax && y >= it->yMin && y <= it->yMax) {
            pisos.erase(std::next(it).base());
            return;
        }
    }
    // 4) Ciudad (zona segura).
    for (std::vector<Ciudad>::iterator it = ciudades.begin(); it != ciudades.end(); ++it) {
        if (x >= it->xMin && x <= it->xMax && y >= it->yMin && y <= it->yMax) {
            ciudades.erase(it);
            return;
        }
    }
}

const std::vector<Posicion>&       EditorMapa::getParedes() const { return paredes; }
const std::vector<Ciudad>&         EditorMapa::getCiudades() const { return ciudades; }
const std::vector<NpcEditor>&      EditorMapa::getNpcs() const { return npcs; }
const std::vector<CriaturaEditor>& EditorMapa::getCriaturas() const { return criaturas; }
const std::vector<ZonaPiso>&       EditorMapa::getPisos() const { return pisos; }

void EditorMapa::cargarDesde(const Mapa& mapa, uint16_t nuevoMapaId) {
    ancho = mapa.getAncho();
    alto = mapa.getAlto();
    mapaId = nuevoMapaId;
    paredes = mapa.getParedes();
    ciudades = mapa.getCiudades();
    pisos = mapa.getPisos();

    npcs.clear();
    for (const auto& [id, npc] : mapa.getSacerdotes()) {
        npcs.push_back(NpcEditor{npc.getId(), TipoNpc::Sacerdote,
                                 npc.getPosicion().x, npc.getPosicion().y});
    }
    for (const auto& [id, npc] : mapa.getComerciantes()) {
        npcs.push_back(NpcEditor{npc.getId(), TipoNpc::Comerciante,
                                 npc.getPosicion().x, npc.getPosicion().y});
    }
    for (const auto& [id, npc] : mapa.getBanqueros()) {
        npcs.push_back(NpcEditor{npc.getId(), TipoNpc::Banquero,
                                 npc.getPosicion().x, npc.getPosicion().y});
    }

    criaturas.clear();
    for (const Criatura& criatura : mapa.obtenerCriaturas()) {
        criaturas.push_back(CriaturaEditor{criatura.getId(), criatura.getTipo(),
                                           criatura.getPos().x, criatura.getPos().y});
    }
}

Mapa EditorMapa::construirMapa() const {
    Mapa mapa(ancho, alto);
    for (const Posicion& p : paredes) {
        mapa.agregarPared(Posicion{p.x, p.y, mapaId});
    }
    for (const Ciudad& c : ciudades) {
        mapa.agregarCiudad(Ciudad{mapaId, c.xMin, c.yMin, c.xMax, c.yMax});
    }
    for (const NpcEditor& n : npcs) {
        mapa.agregarNpc(Npc{n.id, n.tipo, Posicion{n.x, n.y, mapaId}});
    }
    for (const CriaturaEditor& c : criaturas) {
        mapa.agregarCriatura(Criatura{c.id, c.tipo, 0, 0, 0, 0,
                                      Posicion{c.x, c.y, mapaId}, 0, 0, 0, 0});
    }
    for (const ZonaPiso& p : pisos) {
        mapa.agregarPiso(ZonaPiso{mapaId, p.xMin, p.yMin, p.xMax, p.yMax, p.clave});
    }
    return mapa;
}

uint16_t EditorMapa::proximoIdNpc() const {
    uint16_t maximo = 0;
    for (const NpcEditor& n : npcs) {
        if (n.id > maximo) {
            maximo = n.id;
        }
    }
    return static_cast<uint16_t>(maximo + 1);
}

uint16_t EditorMapa::proximoIdCriatura() const {
    uint16_t maximo = 0;
    for (const CriaturaEditor& c : criaturas) {
        if (c.id > maximo) {
            maximo = c.id;
        }
    }
    return static_cast<uint16_t>(maximo + 1);
}
