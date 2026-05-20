#ifndef MAPA_H
#define MAPA_H

#include <cstdint>
#include <map>
#include <optional>

#include "../modelo/posicion.h"
#include "../npc/npc.h"
#include <vector>

struct ItemEnSuelo {
    Posicion posicion;
    uint16_t idItem;
};

class Mapa {
  private:
    uint16_t ancho;
    uint16_t alto;
    std::map<uint16_t, Npc> npcs;
    std::vector<ItemEnSuelo> itemsEnSuelo;

    static bool mismaPosicion(const Posicion& primera, const Posicion& segunda);

public:
    
    Mapa(uint16_t ancho, uint16_t alto);
    void agregarNpc(const Npc& npc);

    bool posicionValida(const Posicion& posicion) const;
    bool hayParedEn(const Posicion& posicion) const;

    std::optional<Npc> buscarNpcCercano(Posicion posicion, TipoNpc tipo) const;
    bool hayNpcCercano(Posicion posicion, TipoNpc tipo) const;
    bool hayNpcEn(const Posicion& posicion) const;
    
    bool hayItemEn(const Posicion& posicion) const;
    bool agregarItem(const Posicion& posicion, uint16_t idItem);
    std::optional<uint16_t> tomarItem(const Posicion& posicion);
    std::vector<ItemEnSuelo> obtenerItemsEnSuelo() const;
};

#endif
