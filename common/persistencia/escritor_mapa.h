#ifndef COMMON_PERSISTENCIA_ESCRITOR_MAPA_H
#define COMMON_PERSISTENCIA_ESCRITOR_MAPA_H

#include <cstdint>
#include <iosfwd>
#include <string>

#include "../game/mapa/mapa.h"
#include "../game/npc/npc.h"

class EscritorMapa {
public:
    void escribir(const Mapa& mapa, uint16_t mapaId, const std::string& path);

private:
    const char* tipoNpcATexto(TipoNpc tipo);

    void escribirNpcs(std::ostream& out, const Mapa& mapa);
};

#endif
