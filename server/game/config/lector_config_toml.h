#ifndef LECTOR_CONFIG_TOML_H
#define LECTOR_CONFIG_TOML_H

#include <string>

#include "config_completa.h"

class LectorConfigToml {
public:
    ConfigCompleta cargar(const std::string& ruta);
};

#endif
