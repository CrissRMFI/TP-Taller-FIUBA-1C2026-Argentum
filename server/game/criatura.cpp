#include "criatura.h"

#include <random>

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
