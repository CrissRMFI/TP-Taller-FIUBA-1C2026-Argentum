#ifndef MAPA_H
#define MAPA_H

#include <cstdint>
#include <map>
#include <optional>
#include <string>
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

struct OroEnSuelo {
  uint32_t cantidad;
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

struct ZonaPiso {
  uint16_t    mapaId;
  uint16_t    xMin;
  uint16_t    yMin;
  uint16_t    xMax;
  uint16_t    yMax;
  std::string clave;
};

class Mapa {
  private:
    uint16_t ancho;
    uint16_t alto;
    std::map<uint16_t, Sacerdote> sacerdotes;
    std::map<uint16_t, Comerciante> comerciantes;
    std::map<uint16_t, Banquero> banqueros;
    std::vector<ItemEnSuelo> itemsEnSuelo;
    std::vector<OroEnSuelo> orosEnSuelo;
    std::vector<Posicion> paredes;
    std::vector<Ciudad> ciudades;
    std::vector<Ciudad> bosques;    // rectangulos de zona boscosa (visual: arboles)
    std::vector<Ciudad> desiertos;  // rectangulos de desierto (visual: arena)
    std::vector<ZonaPiso> pisos;    // zonas de piso visual (pasto/desierto/arboles/...)
    std::map<uint16_t, Criatura> criaturas;

    static bool mismaPosicion(const Posicion& primera, const Posicion& segunda);

    template <typename F>
    void forEachNpc(F&& fn) const {
        for (const auto& [id, npc] : sacerdotes) fn(npc);
        for (const auto& [id, npc] : comerciantes) fn(npc);
        for (const auto& [id, npc] : banqueros) fn(npc);
    }

public:

    Mapa(uint16_t ancho, uint16_t alto);
    bool agregarNpc(const Npc& npc);
    void agregarPared(const Posicion& posicion);

    // Agregan una entrada de stock a TODOS los comerciantes / sacerdotes del
    // mapa. El stock vive por tipo de NPC (todos venden lo mismo); el servidor
    // las invoca al cargar el mapa con los datos de configuracion.
    void agregarStockComerciantes(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta);
    void agregarStockSacerdotes(uint16_t idItem, uint8_t precio);

    uint16_t getAncho() const { return ancho; }
    uint16_t getAlto() const  { return alto; }
    const std::vector<Posicion>& getParedes() const  { return paredes; }
    const std::vector<Ciudad>&   getCiudades() const { return ciudades; }
    const std::vector<Ciudad>&   getBosques() const  { return bosques; }
    const std::vector<Ciudad>&   getDesiertos() const { return desiertos; }
    const std::vector<ZonaPiso>& getPisos() const { return pisos; }
    const std::map<uint16_t, Sacerdote>&   getSacerdotes() const   { return sacerdotes; }
    const std::map<uint16_t, Comerciante>& getComerciantes() const { return comerciantes; }
    const std::map<uint16_t, Banquero>&    getBanqueros() const    { return banqueros; }

    bool posicionValida(const Posicion& posicion) const;
    bool hayParedEn(const Posicion& posicion) const;

    bool hayNpcCercano(const Posicion& posicion, TipoNpc tipo, uint16_t rango) const;
    std::optional<Npc> buscarNpcCercano(const Posicion& posicion, TipoNpc tipo, uint16_t rango) const;
    std::optional<Npc> buscarSacerdoteMasCercano(const Posicion& posicion) const;
    bool hayNpcEn(const Posicion& posicion) const;

    bool hayItemEn(const Posicion& posicion) const;
    bool agregarItem(const Posicion& posicion, uint16_t idItem);

    bool hayOroEn(const Posicion& posicion) const;
    bool agregarOroEnSuelo(const Posicion& posicion, uint32_t cantidad);
    std::optional<uint32_t> tomarOro(const Posicion& posicion);
    std::vector<OroEnSuelo> obtenerOroEnSuelo() const;
    std::vector<OroEnSuelo> actualizarOroEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg);
    void agregarCiudad(const Ciudad &ciudad);
    void agregarBosque(const Ciudad &bosque)   { bosques.push_back(bosque); }
    void agregarDesierto(const Ciudad &desierto) { desiertos.push_back(desierto); }
    void agregarPiso(const ZonaPiso &piso) { pisos.push_back(piso); }
    bool esCiudad(const Posicion &posicion) const;
    bool esZonaSegura(const Posicion &posicion) const;
    std::optional<Npc> buscarNpcEn(const Posicion &posicion) const;
    Sacerdote*         obtenerSacerdote(uint16_t idSacerdote);
    Comerciante*       obtenerComerciante(uint16_t idComerciante);
    Banquero*          obtenerBanquero(uint16_t idBanquero);
    const Sacerdote*   obtenerSacerdote(uint16_t idSacerdote) const;
    const Comerciante* obtenerComerciante(uint16_t idComerciante) const;
    const Banquero*    obtenerBanquero(uint16_t idBanquero) const;
    std::optional<uint16_t> tomarItem(const Posicion &posicion);
    std::vector<ItemEnSuelo> obtenerItemsEnSuelo() const;
    std::optional<Posicion> obtenerPosicionResurreccionCercana(const Posicion &posicion) const;

    bool agregarCriatura(const Criatura& criatura);
    bool removerCriatura(uint16_t idCriatura);
    bool hayCriaturaEn(const Posicion& posicion) const;
    std::optional<Criatura> buscarCriaturaEn(const Posicion& posicion) const;
    Criatura* obtenerCriaturaPor(uint16_t idCriatura);
    const Criatura* obtenerCriaturaPor(uint16_t idCriatura) const;
    std::vector<Criatura> obtenerCriaturas() const;
    bool puedeOcuparCriatura(const Posicion& posicion) const;
    bool moverCriatura(uint16_t idCriatura, const Posicion& destino);
    std::vector<ItemEnSuelo> actualizarItemsEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg);
    size_t cantidadCriaturas() const;

};

#endif
