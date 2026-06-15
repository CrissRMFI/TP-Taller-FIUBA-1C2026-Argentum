#include "lector_mapa.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "error_persistencia.h"
#include "firma_mapa.h"

void LectorMapa::verificarFirma(const toml::table& tabla, const std::string& path) {
    const auto firma = tabla[ARGENTUM_MAPA_FIRMA_CLAVE].value<std::string>();
    if (!firma.has_value() || *firma != ARGENTUM_MAPA_FIRMA) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::FIRMA_INVALIDA,
                path + " (clave '" ARGENTUM_MAPA_FIRMA_CLAVE "' debe ser '"
                        ARGENTUM_MAPA_FIRMA "')");
    }
}

void LectorMapa::validarFirma(const std::string& path) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::TOML_MAL_FORMADO,
                path + " (" + std::string(e.description()) + ")");
    }
    verificarFirma(tbl, path);
}

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
    if (texto == "centinela_piedra")   return TipoCriatura::CentinelaPiedra;
    if (texto == "guerrero_ancestral") return TipoCriatura::GuerreroAncestral;
    if (texto == "aberracion")         return TipoCriatura::Aberracion;
    if (texto == "coloso_roca")        return TipoCriatura::ColosoRoca;
    if (texto == "senor_abismo")       return TipoCriatura::SenorAbismo;
    throw ErrorPersistencia(
            CodigoErrorPersistencia::REGISTRO_INVALIDO,
            path + " (tipo de criatura desconocido: '" + texto + "')");
}

MapaCargado LectorMapa::parsearTabla(const toml::table& tbl, const std::string& path,
                                     const CatalogoCriaturas& catalogoCriaturas,
                                     bool esExterior) {
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
            const TipoCriatura tipoCriatura = tipoCriaturaDesdeTexto(*tipoTexto, path);
            
            if (esExterior && catalogoCriaturas.esSoloMazmorra(tipoCriatura)) {
                std::cerr << "[lector_mapa] " << path << ": criatura '" << *tipoTexto
                          << "' es exclusiva de mazmorra; se ignora en el exterior."
                          << std::endl;
                continue;
            }
            mapa.agregarCriatura(catalogoCriaturas.crear(
                    tipoCriatura, id,
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

    
    if (const toml::array* objetos = tbl["objetos"].as_array()) {
        for (const toml::node& nodo : *objetos) {
            const toml::table* o = nodo.as_table();
            if (o == nullptr) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::REGISTRO_INVALIDO,
                        path + " (entrada de 'objetos' invalida)");
            }
            const auto clave = (*o)["clave"].value<std::string>();
            if (!clave.has_value()) {
                throw ErrorPersistencia(
                        CodigoErrorPersistencia::CLAVE_FALTANTE,
                        path + " (objeto sin clave 'clave')");
            }
            mapa.agregarObjeto(ObjetoMapa{mapaId,
                                          leerUint16(*o, "x", path),
                                          leerUint16(*o, "y", path),
                                          *clave});
        }
    }

    return MapaCargado{std::move(mapa), mapaId, std::nullopt};
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

    verificarFirma(tbl, path);
    MapaCargado cargado = parsearTabla(tbl, path, catalogoCriaturas, /*esExterior=*/false);
    cargado.vinculoMazmorra = leerVinculo(tbl, path);
    return cargado;
}

std::optional<VinculoMazmorra> LectorMapa::leerVinculo(const toml::table& tablaPrincipal,
                                                       const std::string& path) {
    const auto archivo = tablaPrincipal["mazmorra_archivo"].value<std::string>();
    if (!archivo.has_value()) {
        return std::nullopt;
    }
    VinculoMazmorra v;
    v.archivo = *archivo;

    const toml::table* entrada = tablaPrincipal["entrada"].as_table();
    if (entrada == nullptr) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::CLAVE_FALTANTE,
                path + " (mapa con 'mazmorra_archivo' pero sin 'entrada')");
    }
    v.entradaX = leerUint16(*entrada, "x", path);
    v.entradaY = leerUint16(*entrada, "y", path);
    v.entradaDestinoX = leerUint16(*entrada, "destino_x", path);
    v.entradaDestinoY = leerUint16(*entrada, "destino_y", path);

    if (const toml::table* salida = tablaPrincipal["salida"].as_table()) {
        v.salidaX = leerUint16(*salida, "x", path);
        v.salidaY = leerUint16(*salida, "y", path);
        v.salidaDestinoX = leerUint16(*salida, "destino_x", path);
        v.salidaDestinoY = leerUint16(*salida, "destino_y", path);
    } else {
        // Sin salida explicita: la sintetizamos como el retorno natural de la entrada.
        v.salidaX = v.entradaDestinoX;
        v.salidaY = v.entradaDestinoY;
        v.salidaDestinoX = v.entradaX;
        v.salidaDestinoY = v.entradaY;
    }
    return v;
}

void LectorMapa::agregarPortalesMazmorra(const toml::table& tablaPrincipal,
                                         uint16_t mapaPrincipalId,
                                         uint16_t mazmorraId, const std::string& path,
                                         std::vector<Portal>& portales) {
    // Entrada (obligatoria): celda del exterior que transporta a la mazmorra.
    const toml::table* entrada = tablaPrincipal["entrada"].as_table();
    if (entrada == nullptr) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::CLAVE_FALTANTE,
                path + " (mapa con mazmorra pero sin 'entrada')");
    }
    portales.push_back(Portal{
            Posicion{leerUint16(*entrada, "x", path), leerUint16(*entrada, "y", path),
                     mapaPrincipalId},
            Posicion{leerUint16(*entrada, "destino_x", path),
                     leerUint16(*entrada, "destino_y", path), mazmorraId}});

    // Salida (opcional): celda de la mazmorra que devuelve al exterior.
    if (const toml::table* salida = tablaPrincipal["salida"].as_table()) {
        portales.push_back(Portal{
                Posicion{leerUint16(*salida, "x", path), leerUint16(*salida, "y", path),
                         mazmorraId},
                Posicion{leerUint16(*salida, "destino_x", path),
                         leerUint16(*salida, "destino_y", path), mapaPrincipalId}});
    }
}

WorldCargado LectorMapa::leerMundo(const std::string& path,
                                   const CatalogoCriaturas& catalogoCriaturas) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::TOML_MAL_FORMADO,
                path + " (" + std::string(e.description()) + ")");
    }

    verificarFirma(tbl, path);

    WorldCargado mundo;

    // Mapa exterior (raiz del archivo).
    MapaCargado principal = parsearTabla(tbl, path, catalogoCriaturas,true);
    mundo.mapaPrincipalId = principal.mapaId;
    mundo.mapas.emplace(principal.mapaId, std::move(principal.mapa));

    if (const auto archivoRel = tbl["mazmorra_archivo"].value<std::string>()) {
        const std::string pathMaz =
                (std::filesystem::path(path).parent_path() / *archivoRel).string();
        toml::table tblMaz;
        try {
            tblMaz = toml::parse_file(pathMaz);
        } catch (const toml::parse_error& e) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::TOML_MAL_FORMADO,
                    pathMaz + " (" + std::string(e.description()) + ")");
        }
        verificarFirma(tblMaz, pathMaz);

        MapaCargado maz = parsearTabla(tblMaz, pathMaz, catalogoCriaturas, /*esExterior=*/false);
        if (maz.mapaId == mundo.mapaPrincipalId || mundo.mapas.count(maz.mapaId)) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::REGISTRO_INVALIDO,
                    pathMaz + " (mapa_id de mazmorra repetido o igual al exterior: " +
                            std::to_string(maz.mapaId) + ")");
        }

        agregarPortalesMazmorra(tbl, mundo.mapaPrincipalId, maz.mapaId, path, mundo.portales);
        mundo.mapas.emplace(maz.mapaId, std::move(maz.mapa));
    }

    return mundo;
}
