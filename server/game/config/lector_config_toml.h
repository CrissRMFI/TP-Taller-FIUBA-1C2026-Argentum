#ifndef LECTOR_CONFIG_TOML_H
#define LECTOR_CONFIG_TOML_H

#include <string>

#include "config_completa.h"
#include "./ilector_configuracion.h"

class LectorConfigToml : public ILectorConfiguracion {
public:
    ConfigCompleta cargar(const std::string& ruta) override;
};

#endif
