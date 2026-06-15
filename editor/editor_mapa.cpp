#include "editor_mapa.h"

#include <algorithm>

EditorMapa::EditorMapa(uint16_t ancho, uint16_t alto):
        ancho(ancho), alto(alto), mapaId(0), pisoBase("vacio") {}

uint16_t EditorMapa::getAncho() const { return ancho; }
uint16_t EditorMapa::getAlto() const { return alto; }
uint16_t EditorMapa::getMapaId() const { return mapaId; }
void EditorMapa::setMapaId(uint16_t id) { mapaId = id; }
const std::string& EditorMapa::getPisoBase() const { return pisoBase; }

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

bool EditorMapa::hayObjetoEn(uint16_t x, uint16_t y) const {
    for (const ObjetoEditor& o : objetos) {
        if (o.x == x && o.y == y) {
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
    return hayParedEn(x, y) || hayObjetoEn(x, y) || hayNpcEn(x, y) || hayCriaturaEn(x, y);
}

bool EditorMapa::esVacio(uint16_t x, uint16_t y) const {
    return pisoEn(x, y) == "vacio";
}

void EditorMapa::ponerPared(uint16_t x, uint16_t y) {
    // En "vacio" solo se puede pintar piso; nada de paredes/objetos/entidades.
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y) || esVacio(x, y)) {
        return;
    }
    paredes.push_back(Posicion{x, y, mapaId});
}

void EditorMapa::ponerObjeto(const std::string& clave, uint16_t x, uint16_t y) {
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y) || esVacio(x, y)) {
        return;
    }
    objetos.push_back(ObjetoEditor{clave, x, y});
}

void EditorMapa::ponerNpc(TipoNpc tipo, uint16_t x, uint16_t y) {
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y) || esVacio(x, y)) {
        return;
    }
    npcs.push_back(NpcEditor{proximoIdNpc(), tipo, x, y});
}

void EditorMapa::ponerCriatura(TipoCriatura tipo, uint16_t x, uint16_t y) {
    // Las criaturas no pueden ir en zona segura (ciudad): el modelo del juego
    // las rechaza, asi que tampoco las dejamos colocar aca para que el guardado
    // y la recarga sean consistentes. Tampoco sobre "vacio".
    if (!dentroDeLimites(x, y) || celdaOcupada(x, y) || estaEnCiudad(x, y) || esVacio(x, y)) {
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
            if (dentroDeLimites(x, y) && !celdaOcupada(x, y) && !esVacio(x, y)) {
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
    return pisoBase;
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
    // 3) Objeto (arbol, cartel, ...).
    for (std::vector<ObjetoEditor>::iterator it = objetos.begin(); it != objetos.end(); ++it) {
        if (it->x == x && it->y == y) {
            objetos.erase(it);
            return;
        }
    }
    // 4) Zona de piso que cubre la celda (la de mas arriba; revierte a pasto u otra).
    for (std::vector<ZonaPiso>::reverse_iterator it = pisos.rbegin(); it != pisos.rend(); ++it) {
        if (x >= it->xMin && x <= it->xMax && y >= it->yMin && y <= it->yMax) {
            pisos.erase(std::next(it).base());
            return;
        }
    }
    // 5) Ciudad (zona segura).
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
const std::vector<ObjetoEditor>&   EditorMapa::getObjetos() const { return objetos; }

void EditorMapa::redimensionar(uint16_t nuevoAncho, uint16_t nuevoAlto) {
    if (nuevoAncho == 0 || nuevoAlto == 0) {
        return;
    }
    ancho = nuevoAncho;
    alto = nuevoAlto;

    // Al achicar, descartamos las entidades que quedan fuera del nuevo rectangulo.

    const auto fuera = [&](uint16_t x, uint16_t y) { return x >= ancho || y >= alto; };

    paredes.erase(std::remove_if(paredes.begin(), paredes.end(),
                  [&](const Posicion& p) { return fuera(p.x, p.y); }), paredes.end());

    objetos.erase(std::remove_if(objetos.begin(), objetos.end(),
                  [&](const ObjetoEditor& o) { return fuera(o.x, o.y); }), objetos.end());

    npcs.erase(std::remove_if(npcs.begin(), npcs.end(),
               [&](const NpcEditor& n) { return fuera(n.x, n.y); }), npcs.end());

    criaturas.erase(std::remove_if(criaturas.begin(), criaturas.end(),
                    [&](const CriaturaEditor& c) { return fuera(c.x, c.y); }), criaturas.end());

    // Zonas (pisos / ciudades): sacamos las que quedan totalmente afuera y se
    // recortan al nuevo limite las que sobresalen.
    const auto recortarZonas = [&](auto& zonas) {
        zonas.erase(std::remove_if(zonas.begin(), zonas.end(),
                    [&](const auto& z) { return z.xMin >= ancho || z.yMin >= alto; }),
                    zonas.end());
        for (auto& z : zonas) {
            if (z.xMax >= ancho) z.xMax = static_cast<uint16_t>(ancho - 1);
            if (z.yMax >= alto)  z.yMax = static_cast<uint16_t>(alto - 1);
        }
    };
    recortarZonas(pisos);
    recortarZonas(ciudades);
}

void EditorMapa::cargarDesde(const Mapa& mapa, uint16_t nuevoMapaId) {
    ancho = mapa.getAncho();
    alto = mapa.getAlto();
    mapaId = nuevoMapaId;
    paredes = mapa.getParedes();
    ciudades = mapa.getCiudades();
    pisos = mapa.getPisos();

    pisoBase = "pasto";
    for (auto it = pisos.begin(); it != pisos.end(); ++it) {
        if (it->xMin == 0 && it->yMin == 0 &&
            it->xMax == static_cast<uint16_t>(ancho - 1) &&
            it->yMax == static_cast<uint16_t>(alto - 1)) {
            pisoBase = it->clave;
            pisos.erase(it);
            break;
        }
    }

    objetos.clear();
    for (const ObjetoMapa& o : mapa.getObjetos()) {
        objetos.push_back(ObjetoEditor{o.clave, o.x, o.y});
    }

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
    // Relleno base que cubre todo el mapa, debajo de las zonas pintadas (ultima
    // gana). En mapas nuevos es "vacio" (intransitable); en mapas viejos cargados,
    // "pasto", para no convertir en intransitable lo que antes era caminable.
    mapa.agregarPiso(ZonaPiso{mapaId, 0, 0,
                              static_cast<uint16_t>(ancho - 1),
                              static_cast<uint16_t>(alto - 1), pisoBase});
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
    for (const ObjetoEditor& o : objetos) {
        mapa.agregarObjeto(ObjetoMapa{mapaId, o.x, o.y, o.clave});
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
