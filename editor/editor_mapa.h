#ifndef EDITOR_MAPA_H
#define EDITOR_MAPA_H

#include <cstdint>
#include <vector>

#include "common/game/criatura.h"
#include "common/game/mapa/mapa.h"
#include "common/game/modelo/posicion.h"
#include "common/game/npc/npc.h"

//El id se asigna solo (incremental) para que sea unico dentro del mapa.
struct NpcEditor {
    uint16_t id;
    TipoNpc  tipo;
    uint16_t x;
    uint16_t y;
};

struct CriaturaEditor {
    uint16_t     id;
    TipoCriatura tipo;
    uint16_t     x;
    uint16_t     y;
};


class EditorMapa {
private:
    uint16_t ancho;
    uint16_t alto;
    uint16_t mapaId;
    std::vector<Posicion> paredes;
    std::vector<Ciudad> ciudades;
    std::vector<NpcEditor> npcs;
    std::vector<CriaturaEditor> criaturas;

    uint16_t proximoIdNpc() const;
    uint16_t proximoIdCriatura() const;

public:
    EditorMapa(uint16_t ancho, uint16_t alto);

    uint16_t getAncho() const;
    uint16_t getAlto() const;
    uint16_t getMapaId() const;

    bool dentroDeLimites(uint16_t x, uint16_t y) const;
    bool hayParedEn(uint16_t x, uint16_t y) const;
    bool hayNpcEn(uint16_t x, uint16_t y) const;
    bool hayCriaturaEn(uint16_t x, uint16_t y) const;
    bool estaEnCiudad(uint16_t x, uint16_t y) const;
    bool celdaOcupada(uint16_t x, uint16_t y) const;

    void ponerPared(uint16_t x, uint16_t y);
    void ponerNpc(TipoNpc tipo, uint16_t x, uint16_t y);
    void ponerCriatura(TipoCriatura tipo, uint16_t x, uint16_t y);
    void agregarCiudad(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void borrarEn(uint16_t x, uint16_t y);

    const std::vector<Posicion>&       getParedes() const;
    const std::vector<Ciudad>&         getCiudades() const;
    const std::vector<NpcEditor>&      getNpcs() const;
    const std::vector<CriaturaEditor>& getCriaturas() const;

    void cargarDesde(const Mapa& mapa, uint16_t mapaId);
    Mapa construirMapa() const;


};

#endif
