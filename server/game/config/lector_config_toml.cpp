#include "lector_config_toml.h"

#include <toml++/toml.hpp>

// ─── helpers ─────────────────────────────────────────────────────────────────

static StatsRaza cargarRaza(const toml::table& tbl, std::string_view nombre) {
    const auto& r = *tbl["razas"][nombre].as_table();
    return StatsRaza{
        .fVida         = r["f_vida"].value_or(1.0f),
        .fMana         = r["f_mana"].value_or(1.0f),
        .fRecuperacion = r["f_recuperacion"].value_or(1.0f),
        .constitucion  = r["constitucion"].value_or(18),
        .fuerza        = r["fuerza"].value_or(18),
        .agilidad      = r["agilidad"].value_or(15),
        .inteligencia  = r["inteligencia"].value_or(16),
    };
}

static void poblarCatalogo(CatalogoItems& catalogo, const toml::table& tbl) {
    const auto* items_tbl = tbl["items"].as_table();
    if (!items_tbl) return;

    for (auto& [nombre_key, item_node] : *items_tbl) {
        const auto* item = item_node.as_table();
        if (!item) continue;

        uint16_t id = static_cast<uint16_t>((*item)["id"].value_or(0));
        if (id == 0) continue;

        std::string nombre(nombre_key.str());
        std::string tipo = std::string((*item)["tipo"].value_or(std::string_view{}));

        if (tipo == "arma" || tipo == "arma_distancia") {
            catalogo.registrar(id, std::make_unique<Arma>(
                id, nombre,
                static_cast<uint8_t>((*item)["danio_min"].value_or(1)),
                static_cast<uint8_t>((*item)["danio_max"].value_or(2)),
                tipo == "arma_distancia"
            ));
        } else if (tipo == "baculo") {
            std::string hechizo = std::string((*item)["hechizo"].value_or(std::string_view{"misil"}));
            TipoHechizo th = TipoHechizo::Misil;
            if      (hechizo == "curar")         th = TipoHechizo::Curar;
            else if (hechizo == "explosion")     th = TipoHechizo::Explosion;
            else if (hechizo == "flecha_magica") th = TipoHechizo::FlechaMagica;

            catalogo.registrar(id, std::make_unique<Baculo>(
                id, nombre,
                static_cast<uint8_t> ((*item)["danio_min"].value_or(0)),
                static_cast<uint8_t> ((*item)["danio_max"].value_or(0)),
                th,
                static_cast<uint16_t>((*item)["costo_mana"].value_or(0))
            ));
        } else if (tipo == "armadura" || tipo == "casco" || tipo == "escudo") {
            TipoDefensa slot = TipoDefensa::Armadura;
            if      (tipo == "casco")  slot = TipoDefensa::Casco;
            else if (tipo == "escudo") slot = TipoDefensa::Escudo;

            catalogo.registrar(id, std::make_unique<Defensa>(
                id, nombre,
                static_cast<uint8_t>((*item)["defensa_min"].value_or(0)),
                static_cast<uint8_t>((*item)["defensa_max"].value_or(0)),
                slot
            ));
        } else if (tipo == "pocion_vida" || tipo == "pocion_mana") {
            TipoPocion tp = (tipo == "pocion_vida") ? TipoPocion::Vida : TipoPocion::Mana;
            catalogo.registrar(id, std::make_unique<Pocion>(
                id, nombre,
                static_cast<uint16_t>((*item)["cantidad"].value_or(50)),
                tp
            ));
        }
    }
}

// ─── LectorConfigToml ────────────────────────────────────────────────────────

ConfigCompleta LectorConfigToml::cargar(const std::string& ruta) {
    auto tbl = toml::parse_file(ruta);

    ConfigJuego cfg;

    cfg.fVidaGuerrero = tbl["vida"]["f_clase_guerrero"].value_or(3.0f);
    cfg.fVidaPaladin  = tbl["vida"]["f_clase_paladin"].value_or(2.5f);
    cfg.fVidaClerigo  = tbl["vida"]["f_clase_clerigo"].value_or(2.0f);
    cfg.fVidaMago     = tbl["vida"]["f_clase_mago"].value_or(1.5f);

    cfg.fManaGuerrero = tbl["mana"]["f_clase_guerrero"].value_or(0.0f);
    cfg.fManaPaladin  = tbl["mana"]["f_clase_paladin"].value_or(0.8f);
    cfg.fManaClerigo  = tbl["mana"]["f_clase_clerigo"].value_or(1.2f);
    cfg.fManaMago     = tbl["mana"]["f_clase_mago"].value_or(2.0f);

    cfg.fMeditacionPaladin = tbl["mana"]["f_clase_meditacion_paladin"].value_or(1.0f);
    cfg.fMeditacionClerigo = tbl["mana"]["f_clase_meditacion_clerigo"].value_or(2.0f);
    cfg.fMeditacionMago    = tbl["mana"]["f_clase_meditacion_mago"].value_or(3.0f);

    cfg.humano = cargarRaza(tbl, "humano");
    cfg.elfo   = cargarRaza(tbl, "elfo");
    cfg.enano  = cargarRaza(tbl, "enano");
    cfg.gnomo  = cargarRaza(tbl, "gnomo");

    cfg.expLimiteBase = tbl["experiencia"]["limite_base"].value_or(1000);
    cfg.expLimiteExp  = tbl["experiencia"]["limite_exp"].value_or(1.8f);
    cfg.expBonusNivel = tbl["experiencia"]["bonus_nivel"].value_or(10);
    cfg.expKillMax    = tbl["experiencia"]["exp_kill_max"].value_or(0.1f);

    cfg.oroMaxExp     = tbl["oro"]["max_exp"].value_or(1.1f);
    cfg.oroExcesoPct  = tbl["oro"]["exceso_pct"].value_or(0.5f);
    cfg.oroDropNpcMax = tbl["oro"]["drop_npc_max"].value_or(0.2f);

    cfg.esquivarUmbral = tbl["combate"]["esquivar_umbral"].value_or(0.001f);

    cfg.nivelNewbie  = tbl["fair_play"]["nivel_newbie"].value_or(12);
    cfg.maxDiffNivel = tbl["fair_play"]["max_diff_nivel"].value_or(10);

    cfg.clanMaxMiembros = tbl["clanes"]["max_miembros"].value_or(16);
    cfg.clanNivelMinimo = tbl["clanes"]["nivel_minimo"].value_or(6);

    cfg.factorTiempoResurreccion =
        tbl["muerte"]["factor_tiempo_resurreccion"].value_or(0.5f);

    cfg.tickMs = tbl["servidor"]["tick_ms"].value_or(200);

    cfg.vidaInfinita = tbl["cheats"]["vida_infinita"].value_or(false);
    cfg.manaInfinito = tbl["cheats"]["mana_infinito"].value_or(false);
    cfg.invulnerable = tbl["cheats"]["invulnerable"].value_or(false);
    cfg.expX10       = tbl["cheats"]["exp_x10"].value_or(false);

    ConfigCompleta resultado;
    resultado.juego = cfg;
    poblarCatalogo(resultado.items, tbl);
    return resultado;
}
