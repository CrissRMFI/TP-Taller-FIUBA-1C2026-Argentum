#include "editor_mapa.h"

#include <algorithm>

EditorMapa::EditorMapa(uint16_t ancho, uint16_t alto):
        ancho(ancho), alto(alto), mapaId(0) {}

uint16_t EditorMapa::getAncho() const { return ancho; }
uint16_t EditorMapa::getAlto() const { return alto; }
uint16_t EditorMapa::getMapaId() const { return mapaId; }

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

bool EditorMapa::estaEnCiudad(uint16_t x, uint16_t y) const {
    for (const Ciudad& c : ciudades) {
        if (x >= c.xMin && x <= c.xMax && y >= c.yMin && y <= c.yMax) {
            return true;
        }
    }
    return false;
}

void EditorMapa::ponerPared(uint16_t x, uint16_t y) {
    if (!dentroDeLimites(x, y) || hayParedEn(x, y) || hayNpcEn(x, y)) {
        return;
    }
    paredes.push_back(Posicion{x, y, mapaId});
}

void EditorMapa::ponerNpc(TipoNpc tipo, uint16_t x, uint16_t y) {
    if (!dentroDeLimites(x, y) || hayParedEn(x, y) || hayNpcEn(x, y)) {
        return;
    }
    npcs.push_back(NpcEditor{proximoIdNpc(), tipo, x, y});
}

void EditorMapa::agregarCiudad(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    const uint16_t xMin = std::min(x1, x2);
    const uint16_t yMin = std::min(y1, y2);
    const uint16_t xMax = std::max(x1, x2);
    const uint16_t yMax = std::max(y1, y2);
    ciudades.push_back(Ciudad{mapaId, xMin, yMin, xMax, yMax});
}

void EditorMapa::borrarEn(uint16_t x, uint16_t y) {
    for (std::vector<Posicion>::iterator it = paredes.begin(); it != paredes.end(); ++it) {
        if (it->x == x && it->y == y) {
            paredes.erase(it);
            return;
        }
    }
    for (std::vector<NpcEditor>::iterator it = npcs.begin(); it != npcs.end(); ++it) {
        if (it->x == x && it->y == y) {
            npcs.erase(it);
            return;
        }
    }
    for (std::vector<Ciudad>::iterator it = ciudades.begin(); it != ciudades.end(); ++it) {
        if (x >= it->xMin && x <= it->xMax && y >= it->yMin && y <= it->yMax) {
            ciudades.erase(it);
            return;
        }
    }
}

const std::vector<Posicion>&  EditorMapa::getParedes() const { return paredes; }
const std::vector<Ciudad>&    EditorMapa::getCiudades() const { return ciudades; }
const std::vector<NpcEditor>& EditorMapa::getNpcs() const { return npcs; }

void EditorMapa::cargarDesde(const Mapa& mapa, uint16_t nuevoMapaId) {
    ancho = mapa.getAncho();
    alto = mapa.getAlto();
    mapaId = nuevoMapaId;
    paredes = mapa.getParedes();
    ciudades = mapa.getCiudades();

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
