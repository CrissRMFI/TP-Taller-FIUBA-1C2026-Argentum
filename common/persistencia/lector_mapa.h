#ifndef COMMON_PERSISTENCIA_LECTOR_MAPA_H
#define COMMON_PERSISTENCIA_LECTOR_MAPA_H

#include <cstdint>
#include <string>
#include <string_view>

#include <toml++/toml.hpp>

#include "../game/criatura.h"
#include "../game/mapa/mapa.h"
#include "../game/npc/npc.h"
#include "catalogo_criaturas.h"

struct MapaCargado {
    Mapa     mapa;
    uint16_t mapaId;
};

class LectorMapa {
private:
    uint16_t leerUint16(
            const toml::table& tabla, std::string_view clave, const std::string& path);

    TipoNpc tipoNpcDesdeTexto(const std::string& texto, const std::string& path);
    TipoCriatura tipoCriaturaDesdeTexto(const std::string& texto, const std::string& path);

public:
    // 'catalogoCriaturas' aporta los stats por tipo para instanciar las criaturas colocadas en el mapa
    MapaCargado leer(const std::string& path,
                     const CatalogoCriaturas& catalogoCriaturas = CatalogoCriaturas{});

};

#endif
