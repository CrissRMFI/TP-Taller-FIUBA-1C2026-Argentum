#ifndef COMMON_PERSISTENCIA_LECTOR_MAPA_H
#define COMMON_PERSISTENCIA_LECTOR_MAPA_H

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <toml++/toml.hpp>

#include "../game/criatura.h"
#include "../game/mapa/mapa.h"
#include "../game/mapa/portal.h"
#include "../game/npc/npc.h"
#include "catalogo_criaturas.h"
#include "vinculo_mazmorra.h"

struct MapaCargado {
    Mapa mapa;
    uint16_t mapaId;
    std::optional<VinculoMazmorra> vinculoMazmorra;
};

struct WorldCargado {
    std::map<uint16_t, Mapa> mapas;
    std::vector<Portal> portales;
    uint16_t mapaPrincipalId;
};

class LectorMapa {
private:
    uint16_t leerUint16(const toml::table& tabla, std::string_view clave, const std::string& path);

    void verificarFirma(const toml::table& tabla, const std::string& path);

    TipoNpc tipoNpcDesdeTexto(const std::string& texto, const std::string& path);
    TipoCriatura tipoCriaturaDesdeTexto(const std::string& texto, const std::string& path);

    MapaCargado parsearTabla(const toml::table& tabla, const std::string& path,
                             const CatalogoCriaturas& catalogoCriaturas, bool esExterior);

    void agregarPortalesMazmorra(const toml::table& tablaPrincipal, uint16_t mapaPrincipalId,
                                 uint16_t mazmorraId, const std::string& path,
                                 std::vector<Portal>& portales);

    std::optional<VinculoMazmorra> leerVinculo(const toml::table& tablaPrincipal,
                                               const std::string& path);

public:
    void validarFirma(const std::string& path);

    // 'catalogoCriaturas' aporta los stats por tipo para instanciar las criaturas colocadas en el
    // mapa
    MapaCargado leer(const std::string& path,
                     const CatalogoCriaturas& catalogoCriaturas = CatalogoCriaturas{});

    // Carga el escenario completo (exterior + mazmorras + portales). Lo usa el
    // servidor; el cliente/editor siguen usando leer() para el mapa principal.
    WorldCargado leerMundo(const std::string& path,
                           const CatalogoCriaturas& catalogoCriaturas = CatalogoCriaturas{});
};

#endif
