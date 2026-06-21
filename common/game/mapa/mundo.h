#ifndef MUNDO_H
#define MUNDO_H

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <vector>

#include "../modelo/posicion.h"
#include "mapa.h"
#include "portal.h"

// Mundo agrupa los mapas del juego indexados por su mapaId.
class Mundo {
private:
    std::map<uint16_t, Mapa> mapas;
    std::vector<Portal> portales;
    uint16_t principalId;

public:
    // Mundo de un solo mapa (el exterior). Sin portales.
    explicit Mundo(Mapa&& mapaPrincipal, uint16_t mapaId = 0);

    // Mundo con varios mapas (exterior + mazmorras) y los portales que los unen.
    Mundo(std::map<uint16_t, Mapa>&& mapas, std::vector<Portal>&& portales,
          uint16_t mapaPrincipalId);

    bool existeMapa(uint16_t mapaId) const;

    // Si 'origen' es la celda de entrada de un portal, devuelve su destino.
    std::optional<Posicion> destinoPortalEn(const Posicion& origen) const;


    Mapa& mapaDe(const Posicion& posicion);
    const Mapa& mapaDe(const Posicion& posicion) const;
    Mapa& mapaDe(uint16_t mapaId);
    const Mapa& mapaDe(uint16_t mapaId) const;
    Mapa& mapaPrincipal();
    const Mapa& mapaPrincipal() const;

    // Consultas por la posicion
    bool posicionValida(const Posicion& posicion) const;
    bool hayParedEn(const Posicion& posicion) const;
    bool hayObjetoEn(const Posicion& posicion) const;
    bool hayNpcEn(const Posicion& posicion) const;
    bool hayCriaturaEn(const Posicion& posicion) const;
    bool hayItemEn(const Posicion& posicion) const;
    bool hayOroEn(const Posicion& posicion) const;
    bool esCiudad(const Posicion& posicion) const;
    bool esZonaSegura(const Posicion& posicion) const;
    bool puedeOcuparCriatura(const Posicion& posicion) const;
    bool agregarItem(const Posicion& posicion, uint16_t idItem);
    std::optional<uint16_t> tomarItem(const Posicion& posicion);
    std::optional<uint32_t> tomarOro(const Posicion& posicion);
    bool agregarOroEnSuelo(const Posicion& posicion, uint32_t cantidad);
    std::optional<Npc> buscarSacerdoteMasCercano(const Posicion& posicion) const;
    std::optional<Posicion> buscarCeldaLibreCercaDe(
            const Posicion& origen, const std::function<bool(const Posicion&)>& celdaOcupada) const;

    // Criaturas: por id se busca en todos los mapas; alta/movimiento por mapaId
    bool agregarCriatura(const Criatura& criatura);
    bool removerCriatura(uint16_t idCriatura);
    bool moverCriatura(uint16_t idCriatura, const Posicion& destino);
    Criatura* obtenerCriaturaPor(uint16_t idCriatura);
    const Criatura* obtenerCriaturaPor(uint16_t idCriatura) const;

    // Sacerdote por id (busca en todos los mapas)
    Sacerdote* obtenerSacerdote(uint16_t idSacerdote);
    const Sacerdote* obtenerSacerdote(uint16_t idSacerdote) const;

    // Agregados sobre todos los mapas
    std::vector<Criatura> obtenerCriaturas() const;
    std::vector<ItemEnSuelo> obtenerItemsEnSuelo() const;
    std::vector<OroEnSuelo> obtenerOroEnSuelo() const;
    size_t cantidadCriaturas() const;
    size_t cantidadCriaturasEn(uint16_t mapaId) const;
    uint16_t mapaPrincipalId() const;
    std::vector<ItemEnSuelo> actualizarItemsEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg);
    std::vector<OroEnSuelo> actualizarOroEnSuelo(float deltaSegundos, uint16_t tiempoMaximoSeg);
    void agregarStockComerciantes(uint16_t idItem, uint8_t precioCompra, uint8_t precioVenta);
    void agregarStockSacerdotes(uint16_t idItem, uint8_t precio);


    const std::map<uint16_t, Sacerdote>& getSacerdotes() const;
    const std::map<uint16_t, Comerciante>& getComerciantes() const;
    const std::map<uint16_t, Banquero>& getBanqueros() const;
};

#endif
