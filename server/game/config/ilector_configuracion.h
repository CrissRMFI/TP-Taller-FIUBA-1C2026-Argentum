#ifndef ILECTOR_CONFIGURACION_H
#define ILECTOR_CONFIGURACION_H

#include <string>

#include "config_completa.h"

class ILectorConfiguracion {
public:
    virtual ConfigCompleta cargar(const std::string& ruta) = 0;

    virtual ~ILectorConfiguracion() = default;
};

#endif
