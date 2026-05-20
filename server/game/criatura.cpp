#include "criatura.h"

#include <random>

Criatura::Criatura(uint16_t idCriatura, TipoCriatura tipo, uint16_t vidaMaxima, uint8_t nivel, uint8_t fuerza, uint8_t agilidad, Posicion posicion, uint8_t rangoAggro, uint8_t danioMin, uint8_t danioMax)
    : idCriatura(idCriatura),
      tipo(tipo),
      vidaActual(vidaMaxima),
      vidaMaxima(vidaMaxima),
      nivel(nivel),
      fuerza(fuerza),
      agilidad(agilidad),
      posicion(posicion),
      rangoAggro(rangoAggro),
      danioMin(danioMin),
      danioMax(danioMax),
      persiguiendoJugador() {}

uint16_t Criatura::getId() const {
  return idCriatura;
}

Posicion Criatura::getPos() const {
  return posicion;
}

uint8_t Criatura::getAggro() const {
  return rangoAggro;
}

uint8_t Criatura::calcularDanio() const {
  static std::random_device rd;
  static std::mt19937 generador(rd());
  std::uniform_int_distribution<int> distribucion(danioMin, danioMax);
  return static_cast<uint8_t>(distribucion(generador));
}

std::map<uint16_t, bool> Criatura::estaPersiguiendo() const {
  return persiguiendoJugador;
}

void Criatura::mover(Posicion nuevaPosicion) {
  posicion = nuevaPosicion;
}
