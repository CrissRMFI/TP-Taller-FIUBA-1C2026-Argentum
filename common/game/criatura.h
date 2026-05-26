#ifndef CRIATURA_H
#define CRIATURA_H

#include <cstdint>

#include "modelo/posicion.h"

class Aleatorio;

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
    Criatura(uint16_t idCriatura,
             TipoCriatura tipo,
             uint16_t vidaMaxima,
             uint8_t nivel,
             uint8_t fuerza,
             uint8_t agilidad,
             Posicion posicion,
             uint8_t rangoAggro,
             uint8_t danioMin,
             uint8_t danioMax);

    uint16_t getId() const;
    TipoCriatura getTipo() const;
    Posicion getPos() const;
    uint8_t getAggro() const;
    uint8_t getNivel() const;
    uint8_t getAgilidad() const;
    uint16_t getVidaActual() const;
    uint16_t getVidaMaxima() const;
    uint16_t calcularDanio(Aleatorio& aleatorio) const;

    void recibir_danio(uint32_t danio);
    bool esta_muerta() const;

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
};

#endif
