#ifndef COMMON_PERSISTENCIA_ESCRITOR_MAPA_H
#define COMMON_PERSISTENCIA_ESCRITOR_MAPA_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

#include "../game/mapa/mapa.h"
#include "../game/npc/npc.h"
#include "formato_mapa.h"

class EscritorMapa {
public:
    static void escribir(const Mapa& mapa, uint16_t mapaId, const std::string& path);

private:
    static uint32_t saturarA32(std::size_t valor);

    static void escribirBloque(std::ofstream& archivo, const void* datos, std::size_t bytes, const std::string& path);

    static void agregarRegistroNpc(std::vector<NpcRecord>& destino, const Npc& npc);
};

#endif
