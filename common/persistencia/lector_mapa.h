#ifndef COMMON_PERSISTENCIA_LECTOR_MAPA_H
#define COMMON_PERSISTENCIA_LECTOR_MAPA_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>

#include "../game/mapa/mapa.h"

struct MapaCargado {
    Mapa     mapa;
    uint16_t mapaId;
};

class LectorMapa {
public:
    // Lee `path` y reconstruye el Mapa aplicando agregarPared, agregarCiudad
    // y agregarNpc en el orden en que aparecen en el archivo.
    
    static MapaCargado leer(const std::string& path);

private:
    static void leerBloque(std::ifstream& archivo, void* destino, std::size_t bytes, const std::string& path);
};

#endif
