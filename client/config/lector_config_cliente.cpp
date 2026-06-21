#include "lector_config_cliente.h"

#include <cstdint>
#include <string>

#include <toml++/toml.hpp>

#include "../../common/mensajes/mensajes_error_cliente.h"
#include "../registro_cliente.h"

ConfigCliente LectorConfigCliente::cargar(const std::string& path) {
    ConfigCliente cfg;

    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        RegistroCliente::error(std::string("[cliente] ") +
                               MensajesErrorCliente::mensaje(CodigoErrorCliente::CONFIG_NO_LEIDA) +
                               " ('" + path + "': " + std::string(e.description()) + ")");
        return cfg;
    }

    cfg.vsync = tbl["video"]["vsync"].value_or(cfg.vsync);
    cfg.fpsMax = tbl["video"]["fps_max"].value_or(cfg.fpsMax);
    if (cfg.fpsMax <= 0) {
        cfg.fpsMax = 60;
    }

    const int64_t intervalo =
            tbl["movimiento"]["intervalo_ms"].value_or<int64_t>(cfg.intervaloMovimientoMs);
    if (intervalo > 0) {
        cfg.intervaloMovimientoMs = static_cast<uint32_t>(intervalo);
    }

    const int ancho = tbl["video"]["ancho"].value_or(cfg.ancho);
    const int alto = tbl["video"]["alto"].value_or(cfg.alto);
    if (ancho > 0) {
        cfg.ancho = ancho;
    }
    if (alto > 0) {
        cfg.alto = alto;
    }
    cfg.fullscreen = tbl["video"]["fullscreen"].value_or(cfg.fullscreen);

    cfg.fuenteRuta = tbl["chat"]["fuente"].value_or(cfg.fuenteRuta);
    cfg.fondoChatRuta = tbl["chat"]["fondo"].value_or(cfg.fondoChatRuta);
    const int64_t fuenteTam = tbl["chat"]["tam"].value_or<int64_t>(cfg.fuenteTam);
    if (fuenteTam > 0) {
        cfg.fuenteTam = static_cast<int>(fuenteTam);
    }

    const int64_t maxLineas = tbl["chat"]["max_lineas"].value_or<int64_t>(cfg.chatMaxLineas);
    if (maxLineas > 0) {
        cfg.chatMaxLineas = static_cast<int>(maxLineas);
    }

    cfg.chatPanelX = static_cast<int>(tbl["chat"]["panel_x"].value_or<int64_t>(cfg.chatPanelX));
    cfg.chatPanelY = static_cast<int>(tbl["chat"]["panel_y"].value_or<int64_t>(cfg.chatPanelY));
    const int64_t panelAlto = tbl["chat"]["panel_alto"].value_or<int64_t>(cfg.chatPanelAlto);
    if (panelAlto > 0) {
        cfg.chatPanelAlto = static_cast<int>(panelAlto);
    }

    const auto leerColor = [&tbl](const char* seccion, const char* clave,
                                  std::vector<int>& destino) {
        const toml::array* color = tbl[seccion][clave].as_array();
        if (color == nullptr || color->size() != 3) {
            return;
        }
        std::vector<int> rgb;
        for (const toml::node& canal : *color) {
            if (const std::optional<int64_t> valor = canal.value<int64_t>()) {
                rgb.push_back(static_cast<int>(*valor));
            }
        }
        if (rgb.size() == 3) {
            destino = rgb;
        }
    };
    leerColor("chat", "color_texto", cfg.chatColorTexto);
    leerColor("chat", "color_input", cfg.chatColorInput);
    leerColor("chat", "color_ataque", cfg.chatColorAtaque);
    leerColor("chat", "color_hechizo", cfg.chatColorHechizo);
    leerColor("chat", "color_sistema", cfg.chatColorSistema);
    leerColor("chat", "color_experiencia", cfg.chatColorExperiencia);
    leerColor("chat", "color_recuperacion", cfg.chatColorRecuperacion);
    leerColor("chat", "color_critico_hecho", cfg.chatColorCriticoHecho);
    leerColor("chat", "color_critico_recibido", cfg.chatColorCriticoRecibido);

    // --- Nombres de raza/clase (arrays de strings, indexados por enum) ---
    const auto leerNombres = [&tbl](const char* clave, std::vector<std::string>& destino) {
        const toml::array* arr = tbl["personaje"][clave].as_array();
        if (arr == nullptr || arr->empty()) {
            return;
        }
        std::vector<std::string> nombres;
        for (const toml::node& n : *arr) {
            if (const std::optional<std::string> v = n.value<std::string>()) {
                nombres.push_back(*v);
            }
        }
        if (!nombres.empty()) {
            destino = nombres;
        }
    };
    leerNombres("razas", cfg.razasNombres);
    leerNombres("clases", cfg.clasesNombres);

    // --- Panel derecho (inventario/equipo/stats/comercio) ---
    const int64_t panelAncho = tbl["panel"]["ancho"].value_or<int64_t>(cfg.panelAncho);
    if (panelAncho > 0) {
        cfg.panelAncho = static_cast<int>(panelAncho);
    }
    cfg.panelIconDir = tbl["panel"]["icon_dir"].value_or(cfg.panelIconDir);
    cfg.panelFondoCuero = tbl["panel"]["fondo"].value_or(cfg.panelFondoCuero);
    leerColor("panel", "color_texto", cfg.panelColorTexto);
    leerColor("panel", "color_titulo", cfg.panelColorTitulo);

    // --- Banco (calibracion de la grilla sobre es_banco.bmp) ---
    const auto leerInt = [&tbl](const char* clave, int& destino) {
        destino = static_cast<int>(tbl["banco"][clave].value_or<int64_t>(destino));
    };
    leerInt("boveda_x", cfg.bancoBovedaX);
    leerInt("boveda_y", cfg.bancoBovedaY);
    leerInt("inv_x", cfg.bancoInvX);
    leerInt("inv_y", cfg.bancoInvY);
    leerInt("slot", cfg.bancoSlot);
    leerInt("gap", cfg.bancoGap);
    leerInt("cols", cfg.bancoCols);

    // --- Camara (zoom: tiles visibles y limites) ---
    const auto leerCamara = [&tbl](const char* clave, int& destino) {
        const int64_t v = tbl["camara"][clave].value_or<int64_t>(destino);
        if (v > 0) {
            destino = static_cast<int>(v);
        }
    };
    leerCamara("tiles_visibles_x", cfg.camara.tiles_visibles_x);
    leerCamara("tiles_visibles_y", cfg.camara.tiles_visibles_y);
    leerCamara("zoom_min_tiles_x", cfg.camara.zoom_min_tiles_x);
    leerCamara("zoom_min_tiles_y", cfg.camara.zoom_min_tiles_y);
    leerCamara("zoom_max_tiles_x", cfg.camara.zoom_max_tiles_x);
    leerCamara("zoom_max_tiles_y", cfg.camara.zoom_max_tiles_y);
    leerCamara("zoom_paso_tiles", cfg.camara.zoom_paso_tiles);

    cfg.seleccionRango = static_cast<int>(
            tbl["seleccion"]["rango"].value_or<int64_t>(cfg.seleccionRango));
    cfg.seleccionRadioPx = static_cast<int>(
            tbl["seleccion"]["radio_px"].value_or<int64_t>(cfg.seleccionRadioPx));

    return cfg;
}
