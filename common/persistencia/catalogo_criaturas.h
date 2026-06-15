#ifndef COMMON_PERSISTENCIA_CATALOGO_CRIATURAS_H
#define COMMON_PERSISTENCIA_CATALOGO_CRIATURAS_H

#include <cstdint>
#include <map>
#include <string>

#include "../game/criatura.h"
#include "../game/modelo/posicion.h"

// Stats de combate por tipo de criatura, leidos de config/criaturas.toml.
struct StatsCriatura {
    uint16_t vidaMaxima = 0;
    uint8_t  nivel = 0;
    uint8_t  fuerza = 0;
    uint8_t  agilidad = 0;
    uint8_t  rangoAggro = 0;
    uint8_t  danioMin = 0;
    uint8_t  danioMax = 0;
    uint16_t cuerpo = 0;
    uint16_t fxAtaque = 0;
    uint32_t respawnTicks = 0;
    bool respawnFijo = false;
    bool soloMazmorra = false;  // si true, no puede ubicarse en el mapa exterior
};

// Fuente unica de los stats de criatura.
class CatalogoCriaturas {
private:
    std::map<TipoCriatura, StatsCriatura> stats;

    bool tipoDesdeClave(const std::string& clave, TipoCriatura& tipo) const;
public:
    void cargar(const std::string& path);

    bool tiene(TipoCriatura tipo) const;
    bool esSoloMazmorra(TipoCriatura tipo) const;
    StatsCriatura statsDe(TipoCriatura tipo) const;
    Criatura crear(TipoCriatura tipo, uint16_t id, const Posicion& pos) const;

};

#endif
