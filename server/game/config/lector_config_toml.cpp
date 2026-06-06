#include "lector_config_toml.h"

#include <memory>
#include <string>
#include <string_view>
#include <limits>
#include <stdexcept>

#include <toml++/toml.hpp>

static std::string rutaToml(std::string_view seccion, std::string_view clave) {
    std::string ruta(seccion);
    ruta += ".";
    ruta += clave;
    return ruta;
}

static std::string rutaToml(
        std::string_view seccion,
        std::string_view subseccion,
        std::string_view clave) {
    std::string ruta(seccion);
    ruta += ".";
    ruta += subseccion;
    ruta += ".";
    ruta += clave;
    return ruta;
}

[[noreturn]] static void lanzarFaltaConfig(const std::string& ruta) {
    throw std::runtime_error("Falta config TOML obligatoria: " + ruta);
}

static const toml::table& leerTablaObligatoria(
        const toml::table& tbl,
        std::string_view seccion) {
    const toml::table* tabla = tbl[seccion].as_table();
    if (tabla == nullptr) {
        lanzarFaltaConfig(std::string(seccion));
    }
    return *tabla;
}

static const toml::table& leerSubtablaObligatoria(
        const toml::table& tbl,
        std::string_view seccion,
        std::string_view subseccion) {
    const toml::table& tablaSeccion = leerTablaObligatoria(tbl, seccion);
    const toml::table* tabla = tablaSeccion[subseccion].as_table();
    if (tabla == nullptr) {
        std::string ruta(seccion);
        ruta += ".";
        ruta += subseccion;
        lanzarFaltaConfig(ruta);
    }
    return *tabla;
}

static float leerFloatEnTabla(
        const toml::table& tbl,
        std::string_view clave,
        const std::string& ruta) {
    const auto valor = tbl[clave].value<float>();
    if (!valor.has_value()) {
        lanzarFaltaConfig(ruta);
    }
    return *valor;
}

static int leerIntEnTabla(
        const toml::table& tbl,
        std::string_view clave,
        const std::string& ruta) {
    const auto valor = tbl[clave].value<int64_t>();
    if (!valor.has_value()) {
        lanzarFaltaConfig(ruta);
    }
    if (*valor < std::numeric_limits<int>::min() ||
            *valor > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Valor numerico fuera de rango en TOML: " + ruta);
    }
    return static_cast<int>(*valor);
}

static uint16_t leerUint16EnTabla(
        const toml::table& tbl,
        std::string_view clave,
        const std::string& ruta,
        bool permiteCero = false) {
    const auto valor = tbl[clave].value<int64_t>();
    if (!valor.has_value()) {
        lanzarFaltaConfig(ruta);
    }
    if (*valor < (permiteCero ? 0 : 1) || *valor > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("Valor numerico fuera de rango uint16 en TOML: " + ruta);
    }
    return static_cast<uint16_t>(*valor);
}

static uint8_t leerUint8EnTabla(
        const toml::table& tbl,
        std::string_view clave,
        const std::string& ruta,
        bool permiteCero = false) {
    const uint16_t valor = leerUint16EnTabla(tbl, clave, ruta, permiteCero);
    if (valor > std::numeric_limits<uint8_t>::max()) {
        throw std::runtime_error("Valor numerico fuera de rango uint8 en TOML: " + ruta);
    }
    return static_cast<uint8_t>(valor);
}

static bool leerBoolEnTabla(
        const toml::table& tbl,
        std::string_view clave,
        const std::string& ruta) {
    const auto valor = tbl[clave].value<bool>();
    if (!valor.has_value()) {
        lanzarFaltaConfig(ruta);
    }
    return *valor;
}

static std::string leerStringEnTabla(
        const toml::table& tbl,
        std::string_view clave,
        const std::string& ruta) {
    const auto valor = tbl[clave].value<std::string>();
    if (!valor.has_value()) {
        lanzarFaltaConfig(ruta);
    }
    return *valor;
}

static float leerFloatObligatorio(
        const toml::table& tbl,
        std::string_view seccion,
        std::string_view clave) {
    return leerFloatEnTabla(
            leerTablaObligatoria(tbl, seccion),
            clave,
            rutaToml(seccion, clave));
}

static int leerIntObligatorio(
        const toml::table& tbl,
        std::string_view seccion,
        std::string_view clave) {
    return leerIntEnTabla(
            leerTablaObligatoria(tbl, seccion),
            clave,
            rutaToml(seccion, clave));
}

static uint16_t leerUint16Obligatorio(
        const toml::table& tbl,
        std::string_view seccion,
        std::string_view clave) {
    return leerUint16EnTabla(
            leerTablaObligatoria(tbl, seccion),
            clave,
            rutaToml(seccion, clave));
}

static uint8_t leerUint8Obligatorio(
        const toml::table& tbl,
        std::string_view seccion,
        std::string_view clave) {
    return leerUint8EnTabla(
            leerTablaObligatoria(tbl, seccion),
            clave,
            rutaToml(seccion, clave));
}

static bool leerBoolObligatorio(
        const toml::table& tbl,
        std::string_view seccion,
        std::string_view clave) {
    return leerBoolEnTabla(
            leerTablaObligatoria(tbl, seccion),
            clave,
            rutaToml(seccion, clave));
}

static StatsRaza cargarRaza(const toml::table& tbl, std::string_view nombre) {
    const toml::table& r = leerSubtablaObligatoria(tbl, "razas", nombre);

    return StatsRaza{
        .fVida = leerFloatEnTabla(r, "f_vida", rutaToml("razas", nombre, "f_vida")),
        .fMana = leerFloatEnTabla(r, "f_mana", rutaToml("razas", nombre, "f_mana")),
        .fRecuperacion = leerFloatEnTabla(
                r, "f_recuperacion", rutaToml("razas", nombre, "f_recuperacion")),
        .constitucion = leerIntEnTabla(
                r, "constitucion", rutaToml("razas", nombre, "constitucion")),
        .fuerza = leerIntEnTabla(r, "fuerza", rutaToml("razas", nombre, "fuerza")),
        .agilidad = leerIntEnTabla(r, "agilidad", rutaToml("razas", nombre, "agilidad")),
        .inteligencia = leerIntEnTabla(
                r, "inteligencia", rutaToml("razas", nombre, "inteligencia")),
    };
}

static void poblarCatalogo(CatalogoItems& catalogo, const toml::table& tbl) {
    const toml::table& itemsTbl = leerTablaObligatoria(tbl, "items");

    for (auto& [nombreKey, itemNode] : itemsTbl) {
        const std::string nombre(nombreKey.str());
        const auto* item = itemNode.as_table();
        if (!item) {
            if (nombre == "tiempo_suelo_seg") {
                continue;
            }
            throw std::runtime_error("Entrada TOML invalida: items." + nombre);
        }

        const uint16_t id = leerUint16EnTabla(*item, "id", rutaToml("items", nombre, "id"));
        const std::string tipo = leerStringEnTabla(
                *item, "tipo", rutaToml("items", nombre, "tipo"));

        if (tipo == "arma" || tipo == "arma_distancia") {
            catalogo.registrar(
                    id,
                    std::make_unique<Arma>(
                            id,
                            nombre,
                            leerUint8EnTabla(
                                    *item, "danio_min", rutaToml("items", nombre, "danio_min")),
                            leerUint8EnTabla(
                                    *item, "danio_max", rutaToml("items", nombre, "danio_max")),
                            tipo == "arma_distancia"));
        } else if (tipo == "baculo") {
            const std::string hechizo = leerStringEnTabla(
                    *item, "hechizo", rutaToml("items", nombre, "hechizo"));

            TipoHechizo tipoHechizo = TipoHechizo::Misil;
            if (hechizo == "curar") {
                tipoHechizo = TipoHechizo::Curar;
            } else if (hechizo == "explosion") {
                tipoHechizo = TipoHechizo::Explosion;
            } else if (hechizo == "flecha_magica") {
                tipoHechizo = TipoHechizo::FlechaMagica;
            } else if (hechizo != "misil") {
                throw std::runtime_error("Hechizo invalido en TOML: " +
                                         rutaToml("items", nombre, "hechizo"));
            }

            catalogo.registrar(
                    id,
                    std::make_unique<Baculo>(
                            id,
                            nombre,
                            leerUint8EnTabla(
                                    *item, "danio_min", rutaToml("items", nombre, "danio_min"), true),
                            leerUint8EnTabla(
                                    *item, "danio_max", rutaToml("items", nombre, "danio_max"), true),
                            tipoHechizo,
                            leerUint16EnTabla(
                                    *item, "costo_mana",
                                    rutaToml("items", nombre, "costo_mana"), true)));
        } else if (tipo == "armadura" || tipo == "casco" || tipo == "escudo") {
            TipoDefensa slot = TipoDefensa::Armadura;
            if (tipo == "casco") {
                slot = TipoDefensa::Casco;
            } else if (tipo == "escudo") {
                slot = TipoDefensa::Escudo;
            }

            catalogo.registrar(
                    id,
                    std::make_unique<Defensa>(
                            id,
                            nombre,
                            leerUint8EnTabla(
                                    *item, "defensa_min",
                                    rutaToml("items", nombre, "defensa_min"), true),
                            leerUint8EnTabla(
                                    *item, "defensa_max",
                                    rutaToml("items", nombre, "defensa_max"), true),
                            slot));
        } else if (tipo == "pocion_vida" || tipo == "pocion_mana") {
            TipoPocion tipoPocion = (tipo == "pocion_vida") ? TipoPocion::Vida : TipoPocion::Mana;

            catalogo.registrar(
                    id,
                    std::make_unique<Pocion>(
                            id,
                            nombre,
                            leerUint16EnTabla(
                                    *item, "cantidad", rutaToml("items", nombre, "cantidad")),
                            tipoPocion));
        } else {
            throw std::runtime_error("Tipo de item invalido en TOML: " +
                                     rutaToml("items", nombre, "tipo"));
        }
    }
}

void LectorConfigToml::cargarSpawn(const toml::table& tbl, ConfigJuego& cfg) {
    const toml::table& spawn = leerSubtablaObligatoria(tbl, "mapa", "spawn");

    cfg.spawnInicial = Posicion{
        leerUint16EnTabla(spawn, "x",       rutaToml("mapa", "spawn", "x"),       true),
        leerUint16EnTabla(spawn, "y",       rutaToml("mapa", "spawn", "y"),       true),
        leerUint16EnTabla(spawn, "mapa_id", rutaToml("mapa", "spawn", "mapa_id"), true),
    };

    if (cfg.spawnInicial.x >= cfg.mapaAncho || cfg.spawnInicial.y >= cfg.mapaAlto) {
        throw std::runtime_error(
                "mapa.spawn fuera de las dimensiones del mapa (ancho=" +
                std::to_string(cfg.mapaAncho) + ", alto=" +
                std::to_string(cfg.mapaAlto) + ")");
    }
}

void LectorConfigToml::cargarStockNpcs(const toml::table& tbl, ConfigJuego& cfg) {
    const toml::array* itemsComerciante =
            tbl["npcs"]["comerciante"]["items"].as_array();
    if (itemsComerciante == nullptr) {
        lanzarFaltaConfig("npcs.comerciante.items");
    }
    for (const toml::node& nodo : *itemsComerciante) {
        const toml::table* item = nodo.as_table();
        if (item == nullptr) {
            throw std::runtime_error("npcs.comerciante.items: cada item debe ser una tabla");
        }
        cfg.stockComerciante.push_back(EntradaStockComerciante{
                leerUint16EnTabla(*item, "id", "npcs.comerciante.items.id"),
                leerUint8EnTabla(*item, "compra", "npcs.comerciante.items.compra", true),
                leerUint8EnTabla(*item, "venta", "npcs.comerciante.items.venta", true)});
    }

    const toml::array* itemsSacerdote =
            tbl["npcs"]["sacerdote"]["items"].as_array();
    if (itemsSacerdote == nullptr) {
        lanzarFaltaConfig("npcs.sacerdote.items");
    }
    for (const toml::node& nodo : *itemsSacerdote) {
        const toml::table* item = nodo.as_table();
        if (item == nullptr) {
            throw std::runtime_error("npcs.sacerdote.items: cada item debe ser una tabla");
        }
        cfg.stockSacerdote.push_back(EntradaStockSacerdote{
                leerUint16EnTabla(*item, "id", "npcs.sacerdote.items.id"),
                leerUint8EnTabla(*item, "precio", "npcs.sacerdote.items.precio", true)});
    }
}

ConfigCompleta LectorConfigToml::cargar(const std::string& ruta) {
    auto tbl = toml::parse_file(ruta);

    ConfigJuego cfg;

    cfg.fVidaGuerrero = leerFloatObligatorio(tbl, "vida", "f_clase_guerrero");
    cfg.fVidaPaladin = leerFloatObligatorio(tbl, "vida", "f_clase_paladin");
    cfg.fVidaClerigo = leerFloatObligatorio(tbl, "vida", "f_clase_clerigo");
    cfg.fVidaMago = leerFloatObligatorio(tbl, "vida", "f_clase_mago");

    cfg.fManaGuerrero = leerFloatObligatorio(tbl, "mana", "f_clase_guerrero");
    cfg.fManaPaladin = leerFloatObligatorio(tbl, "mana", "f_clase_paladin");
    cfg.fManaClerigo = leerFloatObligatorio(tbl, "mana", "f_clase_clerigo");
    cfg.fManaMago = leerFloatObligatorio(tbl, "mana", "f_clase_mago");

    cfg.fMeditacionPaladin = leerFloatObligatorio(tbl, "mana", "f_clase_meditacion_paladin");
    cfg.fMeditacionClerigo = leerFloatObligatorio(tbl, "mana", "f_clase_meditacion_clerigo");
    cfg.fMeditacionMago = leerFloatObligatorio(tbl, "mana", "f_clase_meditacion_mago");

    cfg.humano = cargarRaza(tbl, "humano");
    cfg.elfo = cargarRaza(tbl, "elfo");
    cfg.enano = cargarRaza(tbl, "enano");
    cfg.gnomo = cargarRaza(tbl, "gnomo");

    cfg.expLimiteBase = leerIntObligatorio(tbl, "experiencia", "limite_base");
    cfg.expLimiteExp = leerFloatObligatorio(tbl, "experiencia", "limite_exp");
    cfg.expBonusNivel = leerIntObligatorio(tbl, "experiencia", "bonus_nivel");
    cfg.expKillMax = leerFloatObligatorio(tbl, "experiencia", "exp_kill_max");

    cfg.oroMaxBase = leerFloatObligatorio(tbl, "oro", "oro_max_base");
    cfg.oroMaxExp = leerFloatObligatorio(tbl, "oro", "max_exp");
    cfg.oroExcesoPct = leerFloatObligatorio(tbl, "oro", "exceso_pct");
    cfg.oroDropNpcMax = leerFloatObligatorio(tbl, "oro", "drop_npc_max");

    cfg.esquivarUmbral = leerFloatObligatorio(tbl, "combate", "esquivar_umbral");
    cfg.probabilidadCritico = leerFloatObligatorio(tbl, "combate", "probabilidad_critico");
    cfg.cooldownAtaqueSeg = leerFloatObligatorio(tbl, "combate", "cooldown_ataque_seg");

    cfg.nivelNewbie = leerIntObligatorio(tbl, "fair_play", "nivel_newbie");
    cfg.maxDiffNivel = leerIntObligatorio(tbl, "fair_play", "max_diff_nivel");

    cfg.clanMaxMiembros = leerIntObligatorio(tbl, "clanes", "max_miembros");
    cfg.clanNivelMinimo = leerIntObligatorio(tbl, "clanes", "nivel_minimo");
    cfg.clanRadioBonus = leerUint16Obligatorio(tbl, "clanes", "radio_bonus");
    cfg.bonusClanPorAliado = leerFloatObligatorio(tbl, "clanes", "bonus_por_aliado");

    cfg.factorTiempoResurreccion =
            leerFloatObligatorio(tbl, "muerte", "factor_tiempo_resurreccion");
    cfg.expPerdidaMuertePct =
            leerFloatObligatorio(tbl, "experiencia", "exp_perdida_muerte_pct");

    cfg.tickMs = leerIntObligatorio(tbl, "servidor", "tick_ms");
    cfg.movimientoJugadorTicks =
            leerUint16Obligatorio(tbl, "servidor", "movimiento_jugador_ticks");

    cfg.rutaJugadores = leerStringEnTabla(
            leerTablaObligatoria(tbl, "persistencia"), "archivo_jugadores",
            rutaToml("persistencia", "archivo_jugadores"));
    cfg.rutaIndiceJugadores = leerStringEnTabla(
            leerTablaObligatoria(tbl, "persistencia"), "archivo_indice",
            rutaToml("persistencia", "archivo_indice"));
    cfg.guardadoSeg = leerIntObligatorio(tbl, "persistencia", "guardado_seg");

    cfg.mapaAncho = leerUint16Obligatorio(tbl, "mapa", "ancho");
    cfg.mapaAlto = leerUint16Obligatorio(tbl, "mapa", "alto");
    cfg.mapaArchivo = leerStringEnTabla(
            leerTablaObligatoria(tbl, "mapa"), "archivo", rutaToml("mapa", "archivo"));
    cargarSpawn(tbl, cfg);

    cfg.rangoInteraccionNpc = leerUint16Obligatorio(tbl, "npcs", "rango_interaccion");
    cfg.cuerpoSacerdote = leerUint16Obligatorio(tbl, "npcs", "cuerpo_sacerdote");
    cfg.cuerpoComerciante = leerUint16Obligatorio(tbl, "npcs", "cuerpo_comerciante");
    cfg.cuerpoBanquero = leerUint16Obligatorio(tbl, "npcs", "cuerpo_banquero");
    cargarStockNpcs(tbl, cfg);
    cfg.rangoVisionAtaque = leerUint16Obligatorio(tbl, "combate", "rango_vision_ataque");

    cfg.vidaInfinita = leerBoolObligatorio(tbl, "cheats", "vida_infinita");
    cfg.manaInfinito = leerBoolObligatorio(tbl, "cheats", "mana_infinito");
    cfg.suicidio = leerBoolObligatorio(tbl, "cheats", "suicidio");
    cfg.invulnerable = leerBoolObligatorio(tbl, "cheats", "invulnerable");
    cfg.expX10 = leerBoolObligatorio(tbl, "cheats", "exp_x10");
    cfg.movimientoCriaturasTicks = leerUint16Obligatorio(tbl, "criaturas", "movimiento_ticks");

    cfg.tiempoItemSueloSeg = leerUint16Obligatorio(tbl, "items", "tiempo_suelo_seg");
    cfg.inventarioMaxItems = leerUint8Obligatorio(tbl, "inventario", "max_items");
    cfg.spawnCriaturasTicks = leerUint16Obligatorio(tbl, "criaturas", "spawn_ticks");
    cfg.poblacionMaxCriaturas = leerUint16Obligatorio(tbl, "criaturas", "poblacion_max");

    cfg.criaturaVidaMaximaBase = leerUint16Obligatorio(tbl, "criaturas", "vida_maxima_base");
    cfg.criaturaNivelBase = leerUint8Obligatorio(tbl, "criaturas", "nivel_base");
    cfg.criaturaFuerzaBase = leerUint8Obligatorio(tbl, "criaturas", "fuerza_base");
    cfg.criaturaAgilidadBase = leerUint8Obligatorio(tbl, "criaturas", "agilidad_base");
    cfg.criaturaRangoAggroBase = leerUint8Obligatorio(tbl, "criaturas", "rango_aggro_base");
    cfg.criaturaDanioMinBase = leerUint8Obligatorio(tbl, "criaturas", "danio_min_base");
    cfg.criaturaDanioMaxBase = leerUint8Obligatorio(tbl, "criaturas", "danio_max_base");

    ConfigCompleta resultado;
    resultado.juego = cfg;
    poblarCatalogo(resultado.items, tbl);

    return resultado;
}
