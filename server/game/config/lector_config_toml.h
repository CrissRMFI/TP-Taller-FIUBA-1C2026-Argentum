#ifndef LECTOR_CONFIG_TOML_H
#define LECTOR_CONFIG_TOML_H

#include <string>

#include "config_juego.h"
#include "../objeto/catalogo_items.h"

struct ConfigCompleta {
    ConfigJuego   juego;
    CatalogoItems items;
};

class LectorConfigToml {
public:
    ConfigCompleta cargar(const std::string& ruta);
};

#endif
