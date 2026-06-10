#include "lector_mapa.h"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

#include "error_persistencia.h"

uint16_t LectorMapa::leerUint16(
        const toml::table& tabla, std::string_view clave, const std::string& path) {
    const auto valor = tabla[clave].value<int64_t>();
    if (!valor.has_value()) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::CLAVE_FALTANTE,
                path + " (clave '" + std::string(clave) + "')");
    }
    if (*valor < 0 || *valor > std::numeric_limits<uint16_t>::max()) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::REGISTRO_INVALIDO,
                path + " (clave '" + std::string(clave) + "' fuera de rango)");
    }
    return static_cast<uint16_t>(*valor);
}

TipoNpc LectorMapa::tipoNpcDesdeTexto(const std::string& texto, const std::string& path) {
    if (texto == "banquero")    return TipoNpc::Banquero;
    if (texto == "comerciante") return TipoNpc::Comerciante;
    if (texto == "sacerdote")   return TipoNpc::Sacerdote;
    throw ErrorPersistencia(
            CodigoErrorPersistencia::NPC_DUPLICADO_O_INVALIDO,
            path + " (tipo de NPC desconocido: '" + texto + "')");
}

TipoCriatura LectorMapa::tipoCriaturaDesdeTexto(const std::string& texto, const std::string& path) {
    if (texto == "goblin")    return TipoCriatura::Goblin;
    if (texto == "esqueleto") return TipoCriatura::Esqueleto;
    if (texto == "zombie")    return TipoCriatura::Zombie;
    if (texto == "arania")    return TipoCriatura::Arania;
    if (texto == "orco")      return TipoCriatura::Orco;
    if (texto == "golem")     return TipoCriatura::Golem;
    throw ErrorPersistencia(
            CodigoErrorPersistencia::REGISTRO_INVALIDO,
            path + " (tipo de criatura desconocido: '" + texto + "')");
}

MapaCargado LectorMapa::leer(const std::string& path,
                             const CatalogoCriaturas& catalogoCriaturas) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::TOML_MAL_FORMADO,
                path + " (" + std::string(e.description()) + ")");
    }

    const uint16_t mapaId = leerUint16(tbl, "mapa_id", path);
    const uint16_t ancho  = leerUint16(tbl, "ancho", path);
    const uint16_t alto   = leerUint16(tbl, "alto", path);

    if (ancho == 0 || alto == 0) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::DIMENSIONES_INVALIDAS, path);
    }

    Mapa mapa(ancho, alto);

    try {
        if (const toml::array* paredes = tbl["paredes"].as_array()) {
            for (const toml::node& nodo : *paredes) {
                const toml::table* p = nodo.as_table();
                if (p == nullptr) {
                    throw ErrorPersistencia(
                            CodigoErrorPersistencia::REGISTRO_INVALIDO,
                            path + " (entrada de 'paredes' invalida)");
                }
                mapa.agregarPared(Posicion{
                        leerUint16(*p, "x", path),
                        leerUint16(*p, "y", path),
                        mapaId});
            }
        }

        if (const toml::array* ciudades = tbl["ciudades"].as_array()) {
            for (const toml::node& nodo : *ciudades) {
                const toml::table* c = nodo.as_table();
                if (c == nullptr) {
                    throw ErrorPersistencia(
                            CodigoErrorPersistencia::REGISTRO_INVALIDO,
                            path + " (entrada de 'ciudades' invalida)");
                }
                mapa.agregarCiudad(Ciudad{
                        mapaId,
                        leerUint16(*c, "x_min", path),
                        leerUint16(*c, "y_min", path),
                        leerUint16(*c, "x_max", path),
                        leerUint16(*c, "y_max", path)});
            }
        }

        const auto leerZonas = [&](const char* clave, void (Mapa::*agregar)(const Ciudad&)) {
            if (const toml::array* zonas = tbl[clave].as_array()) {
                for (const toml::node& nodo : *zonas) {
                    const toml::table* z = nodo.as_table();
                    if (z == nullptr) {
                        throw ErrorPersistencia(CodigoErrorPersistencia::REGISTRO_INVALIDO,
                                                path + " (entrada de zona invalida)");
                    }
                    (mapa.*agregar)(Ciudad{mapaId,
                                           leerUint16(*z, "x_min", path),
                                           leerUint16(*z, "y_min", path),
                                           leerUint16(*z, "x_max", path),
                                           leerUint16(*z, "y_max", path)});
                }
            }
        };
        leerZonas("bosques", &Mapa::agregarBosque);
        leerZonas("desiertos", &Mapa::agregarDesierto);
    } catch (const std::invalid_argument& e) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::REGISTRO_INVALIDO,
                path + " (" + e.what() + ")");
    }

    if (const toml::array* npcs = tbl["npcs"].as_array()) {
        for (const toml::node& nodo : *npcs) {
            const toml::table* n = nodo.as_table();
            if (n == nullptr) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::REGISTRO_INVALIDO,
                        path + " (entrada de 'npcs' invalida)");
            }
            const auto tipoTexto = (*n)["tipo"].value<std::string>();
            if (!tipoTexto.has_value()) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::CLAVE_FALTANTE,
                        path + " (NPC sin clave 'tipo')");
            }
            const uint16_t id = leerUint16(*n, "id", path);
            const Npc npc{
                id,
                tipoNpcDesdeTexto(*tipoTexto, path),
                Posicion{leerUint16(*n, "x", path), leerUint16(*n, "y", path), mapaId}
            };
            if (!mapa.agregarNpc(npc)) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::NPC_DUPLICADO_O_INVALIDO,
                        path + " (id=" + std::to_string(id) + ")");
            }
        }
    }

    // 'criaturas' es opcional: mapas viejos sin la clave siguen cargando.
    if (const toml::array* criaturas = tbl["criaturas"].as_array()) {
        for (const toml::node& nodo : *criaturas) {
            const toml::table* c = nodo.as_table();
            if (c == nullptr) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::REGISTRO_INVALIDO,
                        path + " (entrada de 'criaturas' invalida)");
            }
            const auto tipoTexto = (*c)["tipo"].value<std::string>();
            if (!tipoTexto.has_value()) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::CLAVE_FALTANTE,
                        path + " (criatura sin clave 'tipo')");
            }
            const uint16_t id = leerUint16(*c, "id", path);
            mapa.agregarCriatura(catalogoCriaturas.crear(
                    tipoCriaturaDesdeTexto(*tipoTexto, path), id,
                    Posicion{leerUint16(*c, "x", path), leerUint16(*c, "y", path), mapaId}));
        }
    }

    // 'pisos' es opcional: mapas viejos sin la clave siguen cargando.
    if (const toml::array* pisos = tbl["pisos"].as_array()) {
        for (const toml::node& nodo : *pisos) {
            const toml::table* p = nodo.as_table();
            if (p == nullptr) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::REGISTRO_INVALIDO,
                        path + " (entrada de 'pisos' invalida)");
            }
            const auto clave = (*p)["clave"].value<std::string>();
            if (!clave.has_value()) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::CLAVE_FALTANTE,
                        path + " (piso sin clave 'clave')");
            }
            mapa.agregarPiso(ZonaPiso{mapaId,
                                      leerUint16(*p, "x_min", path),
                                      leerUint16(*p, "y_min", path),
                                      leerUint16(*p, "x_max", path),
                                      leerUint16(*p, "y_max", path),
                                      *clave});
        }
    }

    return MapaCargado{std::move(mapa), mapaId};
}
