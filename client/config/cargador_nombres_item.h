#ifndef CARGADOR_NOMBRES_ITEM_H
#define CARGADOR_NOMBRES_ITEM_H

#include <cstdint>
#include <string>
#include <unordered_map>

// Lee la seccion [items] del TOML de configuracion del juego y arma el mapeo
class CargadorNombresItem {
public:
    std::unordered_map<std::string, uint16_t> cargar(const std::string& path) const;
};

#endif
