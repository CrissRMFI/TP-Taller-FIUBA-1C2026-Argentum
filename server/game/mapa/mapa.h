#ifndef MAPA_H
#define MAPA_H

#include <cstdint>
#include <map>
#include <optional>

#include "../modelo/posicion.h"
#include "../npc/npc.h"

class Mapa {
private:
    std::map<uint16_t, Npc> npcs;

    static bool mismaPosicion(const Posicion& primera, const Posicion& segunda);

public:
    void agregarNpc(const Npc& npc);

    bool posicionValida(const Posicion& posicion) const;
    bool hayParedEn(const Posicion& posicion) const;

    std::optional<Npc> buscarNpcCercano(Posicion posicion, TipoNpc tipo) const;
    bool hayNpcCercano(Posicion posicion, TipoNpc tipo) const;
    bool hayNpcEn(const Posicion& posicion) const;
};

#endif
