#ifndef CRIATURA_H
#define CRIATURA_H

#include <cstdint>
#include <map>

#include "modelo/posicion.h"

enum class TipoCriatura {
    Goblin,
    Esqueleto,
    Zombie,
    Arania,
    Orco,
    Golem
};

class Criatura {
public:
    Criatura(uint16_t idCriatura, TipoCriatura tipo, uint16_t vidaMaxima, uint8_t nivel, uint8_t fuerza, uint8_t agilidad, Posicion posicion, uint8_t rangoAggro, uint8_t danioMin, uint8_t danioMax);

    uint16_t getId() const;
    Posicion getPos() const;
    uint8_t getAggro() const;
    uint16_t calcularDanio() const;
    std::map<uint16_t, bool> estaPersiguiendo() const;
    void mover(Posicion nuevaPosicion);

private:
    uint16_t idCriatura;
    TipoCriatura tipo;
    uint16_t vidaActual;
    uint16_t vidaMaxima;
    uint8_t nivel;
    uint8_t fuerza;
    uint8_t agilidad;
    Posicion posicion;
    uint8_t rangoAggro;
    uint8_t danioMin;
    uint8_t danioMax;
    std::map<uint16_t, bool> persiguiendoJugador;
};

#endif
