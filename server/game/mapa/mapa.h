#ifndef MAPA_H
#define MAPA_H

#include <cstdint>
#include <map>
#include <optional>
#include <vector>
#include "../modelo/posicion.h"
#include "../npc/npc.h"
#include "../npc/sacerdote.h"
#include "../npc/comerciante.h"
#include "../npc/banquero.h"
#include "../criatura.h"

struct ItemEnSuelo {
  uint16_t idItem;
  Posicion posicion;
  float segundosEnSuelo;
};

struct Ciudad {
  uint16_t mapaId;
  uint16_t xMin;
  uint16_t yMin;
  uint16_t xMax;
  uint16_t yMax;
};

class Mapa {
  private:
    uint16_t ancho;
    uint16_t alto;
    std::map<uint16_t, Npc> npcs;
    std::map<uint16_t, Sacerdote> sacerdotes;
    std::map<uint16_t, Comerciante> comerciantes;
    std::map<uint16_t, Banquero> banqueros;
    std::vector<ItemEnSuelo> itemsEnSuelo;
    std::vector<Posicion> paredes;
    std::vector<Ciudad> ciudades;
    std::map<uint16_t, Criatura> criaturas;

    static bool mismaPosicion(const Posicion& primera, const Posicion& segunda);

public:
    
    Mapa(uint16_t ancho, uint16_t alto);
    void agregarNpc(const Npc& npc);
    void agregarPared(const Posicion& posicion);

    bool posicionValida(const Posicion& posicion) const;
    bool hayParedEn(const Posicion& posicion) const;

    bool hayNpcCercano(const Posicion& posicion, TipoNpc tipo, uint16_t rango) const;
    std::optional<Npc> buscarNpcCercano(const Posicion& posicion, TipoNpc tipo, uint16_t rango) const;
    std::optional<Npc> buscarSacerdoteMasCercano(const Posicion& posicion) const;
    bool hayNpcEn(const Posicion& posicion) const;
    
    bool hayItemEn(const Posicion& posicion) const;
    bool agregarItem(const Posicion& posicion, uint16_t idItem);
    void agregarCiudad(const Ciudad &ciudad);
    bool esCiudad(const Posicion &posicion) const;
    bool esZonaSegura(const Posicion &posicion) const;
    std::optional<Npc> buscarNpcEn(const Posicion &posicion) const;
    std::vector<Npc> obtenerNpcs() const;
    std::optional<Sacerdote> obtenerSacerdote(uint16_t idSacerdote) const;
    std::optional<Comerciante> obtenerComerciante(uint16_t idComerciante) const;
    std::optional<Banquero> obtenerBanquero(uint16_t idBanquero) const;
    std::vector<Npc> obtenerNpcsPorTipo(TipoNpc tipo) const;
    std::optional<uint16_t> tomarItem(const Posicion &posicion);
    std::vector<ItemEnSuelo> obtenerItemsEnSuelo() const;
    std::optional<Posicion> obtenerPosicionResurreccionCercana(const Posicion &posicion) const;

    void agregarCriatura(const Criatura& criatura);
    bool hayCriaturaEn(const Posicion& posicion) const;
    std::optional<Criatura> buscarCriaturaEn(const Posicion& posicion) const;
    std::vector<Criatura> obtenerCriaturas() const;
    bool puedeOcuparCriatura(const Posicion& posicion) const;
    void moverCriatura(uint16_t idCriatura, const Posicion& destino);
    std::vector<ItemEnSuelo> actualizarItemsEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg);
    size_t cantidadCriaturas() const;

};

#endif
