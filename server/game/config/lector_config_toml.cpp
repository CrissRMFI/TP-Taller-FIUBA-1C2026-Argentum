#include "lector_config_toml.h"

#include <fstream>
#include <stdexcept>

std::string ParserTOML::trim(const std::string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) return "";
    size_t fin = s.find_last_not_of(" \t\r\n");
    return s.substr(ini, fin - ini + 1);
}

std::string ParserTOML::sinComentario(const std::string& linea) {
    size_t pos = linea.find('#');
    return pos != std::string::npos ? linea.substr(0, pos) : linea;
}

void ParserTOML::parsear(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open())
        throw std::runtime_error("No se pudo abrir el archivo de configuracion: " + ruta);

    std::string seccion;
    std::string linea;

    while (std::getline(archivo, linea)) {
        linea = trim(sinComentario(linea));
        if (linea.empty()) continue;

        if (linea.front() == '[') {
            size_t cierre = linea.find(']');
            if (cierre == std::string::npos) continue;
            seccion = trim(linea.substr(1, cierre - 1));
        } else {
            size_t eq = linea.find('=');
            if (eq == std::string::npos) continue;
            std::string clave = trim(linea.substr(0, eq));
            std::string valor = trim(linea.substr(eq + 1));
            std::string claveCompleta = seccion.empty() ? clave : seccion + "." + clave;
            datos[claveCompleta] = valor;
        }
    }
}

float ParserTOML::getFloat(const std::string& clave, float defecto) const {
    auto it = datos.find(clave);
    if (it == datos.end()) return defecto;
    try { return std::stof(it->second); } catch (...) { return defecto; }
}

int ParserTOML::getInt(const std::string& clave, int defecto) const {
    auto it = datos.find(clave);
    if (it == datos.end()) return defecto;
    try { return std::stoi(it->second); } catch (...) { return defecto; }
}

bool ParserTOML::getBool(const std::string& clave, bool defecto) const {
    auto it = datos.find(clave);
    if (it == datos.end()) return defecto;
    return it->second == "true";
}

// ---- LectorConfigToml -------------------------------------------------------

StatsRaza LectorConfigToml::cargarRaza(const ParserTOML& p, const std::string& nombre) {
    return StatsRaza{
        .fVida         = p.getFloat("razas." + nombre + ".f_vida",         1.0f),
        .fMana         = p.getFloat("razas." + nombre + ".f_mana",         1.0f),
        .fRecuperacion = p.getFloat("razas." + nombre + ".f_recuperacion", 1.0f),
        .constitucion  = p.getInt  ("razas." + nombre + ".constitucion",   18),
        .fuerza        = p.getInt  ("razas." + nombre + ".fuerza",         18),
        .agilidad      = p.getInt  ("razas." + nombre + ".agilidad",       15),
        .inteligencia  = p.getInt  ("razas." + nombre + ".inteligencia",   16),
    };
}

ConfigJuego LectorConfigToml::cargar(const std::string& ruta) {
    ParserTOML p;
    p.parsear(ruta);

    ConfigJuego cfg;

    cfg.fVidaGuerrero = p.getFloat("vida.f_clase_guerrero", 3.0f);
    cfg.fVidaPaladin  = p.getFloat("vida.f_clase_paladin",  2.5f);
    cfg.fVidaClerigo  = p.getFloat("vida.f_clase_clerigo",  2.0f);
    cfg.fVidaMago     = p.getFloat("vida.f_clase_mago",     1.5f);

    cfg.fManaGuerrero = p.getFloat("mana.f_clase_guerrero", 0.0f);
    cfg.fManaPaladin  = p.getFloat("mana.f_clase_paladin",  0.8f);
    cfg.fManaClerigo  = p.getFloat("mana.f_clase_clerigo",  1.2f);
    cfg.fManaMago     = p.getFloat("mana.f_clase_mago",     2.0f);

    cfg.fMeditacionPaladin = p.getFloat("mana.f_clase_meditacion_paladin", 1.0f);
    cfg.fMeditacionClerigo = p.getFloat("mana.f_clase_meditacion_clerigo", 2.0f);
    cfg.fMeditacionMago    = p.getFloat("mana.f_clase_meditacion_mago",    3.0f);

    cfg.humano = cargarRaza(p, "humano");
    cfg.elfo   = cargarRaza(p, "elfo");
    cfg.enano  = cargarRaza(p, "enano");
    cfg.gnomo  = cargarRaza(p, "gnomo");

    cfg.expLimiteBase = p.getInt  ("experiencia.limite_base",  1000);
    cfg.expLimiteExp  = p.getFloat("experiencia.limite_exp",   1.8f);
    cfg.expBonusNivel = p.getInt  ("experiencia.bonus_nivel",  10);
    cfg.expKillMax    = p.getFloat("experiencia.exp_kill_max", 0.1f);

    cfg.oroMaxExp     = p.getFloat("oro.max_exp",      1.1f);
    cfg.oroExcesoPct  = p.getFloat("oro.exceso_pct",   0.5f);
    cfg.oroDropNpcMax = p.getFloat("oro.drop_npc_max", 0.2f);

    cfg.esquivarUmbral = p.getFloat("combate.esquivar_umbral", 0.001f);

    cfg.nivelNewbie  = p.getInt("fair_play.nivel_newbie",   12);
    cfg.maxDiffNivel = p.getInt("fair_play.max_diff_nivel", 10);

    cfg.clanMaxMiembros = p.getInt("clanes.max_miembros", 16);
    cfg.clanNivelMinimo = p.getInt("clanes.nivel_minimo",  6);

    cfg.factorTiempoResurreccion =
        p.getFloat("muerte.factor_tiempo_resurreccion", 0.5f);

    cfg.vidaInfinita = p.getBool("cheats.vida_infinita", false);
    cfg.manaInfinito = p.getBool("cheats.mana_infinito", false);
    cfg.invulnerable = p.getBool("cheats.invulnerable",  false);
    cfg.expX10       = p.getBool("cheats.exp_x10",       false);

    return cfg;
}
