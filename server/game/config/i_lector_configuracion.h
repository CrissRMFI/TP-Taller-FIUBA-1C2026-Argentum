#ifndef I_LECTOR_CONFIGURACION_H
#define I_LECTOR_CONFIGURACION_H

#include <string>
#include "config_juego.h"

class ILectorConfiguracion {
public:
    virtual ConfigJuego cargar(const std::string& ruta) = 0;
    virtual ~ILectorConfiguracion() = default;
};

#endif
