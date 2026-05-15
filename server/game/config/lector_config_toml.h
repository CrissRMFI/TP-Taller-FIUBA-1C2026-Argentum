#ifndef LECTOR_CONFIG_TOML_H
#define LECTOR_CONFIG_TOML_H

#include <map>
#include <string>

#include "i_lector_configuracion.h"

class ParserTOML {
    std::map<std::string, std::string> datos;

    static std::string trim(const std::string& s);
    static std::string sinComentario(const std::string& linea);

public:
    void  parsear(const std::string& ruta);
    float getFloat(const std::string& clave, float defecto) const;
    int   getInt(const std::string& clave, int defecto) const;
    bool  getBool(const std::string& clave, bool defecto) const;
};

class LectorConfigToml : public ILectorConfiguracion {
    static StatsRaza cargarRaza(const ParserTOML& p, const std::string& nombre);

public:
    ConfigJuego cargar(const std::string& ruta) override;
};

#endif
