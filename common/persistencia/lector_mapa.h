#ifndef COMMON_PERSISTENCIA_LECTOR_MAPA_H
#define COMMON_PERSISTENCIA_LECTOR_MAPA_H

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <toml++/toml.hpp>

#include "../game/criatura.h"
#include "../game/mapa/mapa.h"
#include "../game/mapa/portal.h"
#include "../game/npc/npc.h"
#include "catalogo_criaturas.h"

struct MapaCargado {
    Mapa     mapa;
    uint16_t mapaId;
};

// Resultado de cargar el escenario completo: el mapa exterior mas las mazmorras,
// indexadas por su mapaId, y los portales que las vinculan. 'mapaPrincipalId' es
// el id del exterior (donde aparecen los jugadores).
struct WorldCargado {
    std::map<uint16_t, Mapa> mapas;
    std::vector<Portal>      portales;
    uint16_t                 mapaPrincipalId;
};

class LectorMapa {
private:
    uint16_t leerUint16(
            const toml::table& tabla, std::string_view clave, const std::string& path);

    TipoNpc tipoNpcDesdeTexto(const std::string& texto, const std::string& path);
    TipoCriatura tipoCriaturaDesdeTexto(const std::string& texto, const std::string& path);

    // Construye un Mapa (con su mapaId) a partir de una tabla TOML: sirve tanto
    // para el mapa raiz como para cada sub-tabla [[mazmorra]].
    MapaCargado parsearTabla(const toml::table& tabla, const std::string& path,
                             const CatalogoCriaturas& catalogoCriaturas);

    // Arma los dos portales (entrada exterior->mazmorra y salida mazmorra->exterior)
    // a partir de la sub-tabla [[mazmorra]]. 'mazmorraId' es el id de esa mazmorra.
    void agregarPortalesMazmorra(const toml::table& mazmorra, uint16_t mapaPrincipalId,
                                 uint16_t mazmorraId, const std::string& path,
                                 std::vector<Portal>& portales);

public:
    // 'catalogoCriaturas' aporta los stats por tipo para instanciar las criaturas colocadas en el mapa
    MapaCargado leer(const std::string& path,
                     const CatalogoCriaturas& catalogoCriaturas = CatalogoCriaturas{});

    // Carga el escenario completo (exterior + mazmorras + portales). Lo usa el
    // servidor; el cliente/editor siguen usando leer() para el mapa principal.
    WorldCargado leerMundo(const std::string& path,
                           const CatalogoCriaturas& catalogoCriaturas = CatalogoCriaturas{});

};

#endif
