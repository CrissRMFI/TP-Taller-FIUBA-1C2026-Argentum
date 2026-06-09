#ifndef COMMON_PERSISTENCIA_LECTOR_MAPA_H
#define COMMON_PERSISTENCIA_LECTOR_MAPA_H

#include <cstdint>
#include <string>
#include <string_view>

#include <toml++/toml.hpp>

#include "../game/mapa/mapa.h"
#include "../game/npc/npc.h"

struct MapaCargado {
    Mapa     mapa;
    uint16_t mapaId;
};

class LectorMapa {
public:
    MapaCargado leer(const std::string& path);

private:
    uint16_t leerUint16(
            const toml::table& tabla, std::string_view clave, const std::string& path);

    TipoNpc tipoNpcDesdeTexto(const std::string& texto, const std::string& path);
};

#endif
