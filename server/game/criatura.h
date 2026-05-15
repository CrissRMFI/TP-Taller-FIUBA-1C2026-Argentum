#ifndef CRIATURA_H
#define CRIATURA_H

#include <string>
#include <cstdint>
#include <vector>

// Pongo aca para que sea mas facil de manejar, luego hay que moverlos
enum class TipoCriatura {
    Goblin,
    Esqueleto,
    Zombie,
    Arania,
    Orco,
    Golem
};

struct Posicion {
    uint16_t x;
    uint16_t y;
    uint16_t mapaId;
};

class Criatura {
public:
    friend class Juego; // Para que el juego pueda modificar si esta persiguiendo a un jugador
    uint16_t getId() const;
    Posicion getPos() const;
    uint8_t getAggro() const;
    uint8_t calcularDanio() const;
    std::vector<bool, uint16_t> estaPersiguiendo();
    void mover(Posicion& nuevaPosicion) const; // El juego mueve a la criatura dependiendo si esta cerca del jugador o no

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
    std::vector<bool, uint16_t> persiguiendoJugador;
    
};

#endif // CRIATURA_H