#include "cargador_nombres_item.h"

#include <iostream>

#include <toml++/toml.hpp>

std::unordered_map<std::string, uint16_t> CargadorNombresItem::cargar(
        const std::string& path) const {
    std::unordered_map<std::string, uint16_t> nombres;

    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        std::cerr << "No se pudo leer '" << path << "' (" << e.description()
                  << "). El mini-chat no podra resolver nombres de objetos." << std::endl;
        return nombres;
    }

    const toml::table* items = tbl["items"].as_table();
    if (items == nullptr) {
        return nombres;
    }

    // Cada subtabla de [items] que tenga un 'id' entero es un objeto del juego.
    // Las claves sin subtabla (p.ej. tiempos) simplemente se ignoran.
    for (const auto& [clave, valor] : *items) {
        const toml::table* item = valor.as_table();
        if (item == nullptr) {
            continue;
        }
        const std::optional<int64_t> id = (*item)["id"].value<int64_t>();
        if (id && *id >= 0 && *id <= 0xFFFF) {
            nombres[std::string(clave.str())] = static_cast<uint16_t>(*id);
        }
    }

    return nombres;
}
