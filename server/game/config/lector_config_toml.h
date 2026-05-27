#ifndef LECTOR_CONFIG_TOML_H
#define LECTOR_CONFIG_TOML_H

#include <string>

#include <toml++/toml.hpp>

#include "config_completa.h"
#include "./ilector_configuracion.h"

class LectorConfigToml : public ILectorConfiguracion {
public:
    ConfigCompleta cargar(const std::string& ruta) override;

private:
    // Lee y valida el bloque [mapa.spawn] dejando el resultado en cfg.spawnInicial. Aislado para no engordar cargar() y para tener un lugar donde crezca la validacion (por ejemplo, evitar paredes/zonas no seguras) sin romper la firma publica.
    void cargarSpawn(const toml::table& tbl, ConfigJuego& cfg);
};

#endif
