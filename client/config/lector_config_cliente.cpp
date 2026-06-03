#include "lector_config_cliente.h"

#include <cstdint>
#include <iostream>

#include <toml++/toml.hpp>

ConfigCliente LectorConfigCliente::cargar(const std::string& path) {
    ConfigCliente cfg;

    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        std::cerr << "No se pudo leer '" << path << "' (" << e.description()
                  << "). Se usan valores por defecto del cliente." << std::endl;
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

    return cfg;
}
