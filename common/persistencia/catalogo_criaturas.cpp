#include "catalogo_criaturas.h"

#include <cstdint>

#include <toml++/toml.hpp>

#include "error_persistencia.h"

bool CatalogoCriaturas::tipoDesdeClave(const std::string& clave, TipoCriatura& tipo) const {
    if (clave == "goblin")    { tipo = TipoCriatura::Goblin;    return true; }
    if (clave == "esqueleto") { tipo = TipoCriatura::Esqueleto; return true; }
    if (clave == "zombie")    { tipo = TipoCriatura::Zombie;    return true; }
    if (clave == "arania")    { tipo = TipoCriatura::Arania;    return true; }
    if (clave == "orco")      { tipo = TipoCriatura::Orco;      return true; }
    if (clave == "golem")     { tipo = TipoCriatura::Golem;     return true; }
    return false;
}

void CatalogoCriaturas::cargar(const std::string& path) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::TOML_MAL_FORMADO,
                path + " (" + std::string(e.description()) + ")");
    }

    stats.clear();

    const toml::array* lista = tbl["criatura"].as_array();
    if (lista == nullptr) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::CLAVE_FALTANTE,
                path + " (falta el arreglo 'criatura')");
    }

    for (const toml::node& nodo : *lista) {
        const toml::table* t = nodo.as_table();
        if (t == nullptr) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::REGISTRO_INVALIDO,
                    path + " (entrada de 'criatura' invalida)");
        }
        const auto clave = (*t)["clave"].value<std::string>();
        TipoCriatura tipo;
        if (!clave.has_value() || !tipoDesdeClave(*clave, tipo)) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::REGISTRO_INVALIDO,
                    path + " (criatura con 'clave' faltante o desconocida)");
        }

        StatsCriatura s;
        s.vidaMaxima = static_cast<uint16_t>((*t)["vida_maxima"].value_or<int64_t>(0));
        s.nivel      = static_cast<uint8_t>((*t)["nivel"].value_or<int64_t>(0));
        s.fuerza     = static_cast<uint8_t>((*t)["fuerza"].value_or<int64_t>(0));
        s.agilidad   = static_cast<uint8_t>((*t)["agilidad"].value_or<int64_t>(0));
        s.rangoAggro = static_cast<uint8_t>((*t)["rango_aggro"].value_or<int64_t>(0));
        s.danioMin   = static_cast<uint8_t>((*t)["danio_min"].value_or<int64_t>(0));
        s.danioMax   = static_cast<uint8_t>((*t)["danio_max"].value_or<int64_t>(0));
        s.cuerpo     = static_cast<uint16_t>((*t)["cuerpo"].value_or<int64_t>(0));
        stats[tipo] = s;
    }
}

bool CatalogoCriaturas::tiene(TipoCriatura tipo) const {
    return stats.find(tipo) != stats.end();
}

StatsCriatura CatalogoCriaturas::statsDe(TipoCriatura tipo) const {
    const auto it = stats.find(tipo);
    return (it != stats.end()) ? it->second : StatsCriatura{};
}

Criatura CatalogoCriaturas::crear(TipoCriatura tipo, uint16_t id, const Posicion& pos) const {
    const StatsCriatura s = statsDe(tipo);
    const uint8_t danioMax = s.danioMax < s.danioMin ? s.danioMin : s.danioMax;
    return Criatura{id, tipo, s.vidaMaxima, s.nivel, s.fuerza, s.agilidad,
                    pos, s.rangoAggro, s.danioMin, danioMax, s.cuerpo};
}
