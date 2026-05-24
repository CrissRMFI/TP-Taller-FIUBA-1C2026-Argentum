#include "criatura.h"

#include <random>

Criatura::Criatura(uint16_t idCriatura,
                   TipoCriatura tipo,
                   uint16_t vidaMaxima,
                   uint8_t nivel,
                   uint8_t fuerza,
                   uint8_t agilidad,
                   Posicion posicion,
                   uint8_t rangoAggro,
                   uint8_t danioMin,
                   uint8_t danioMax)
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

TipoCriatura Criatura::getTipo() const {
    return tipo;
}

Posicion Criatura::getPos() const {
    return posicion;
}

uint8_t Criatura::getAggro() const {
    return rangoAggro;
}

uint8_t Criatura::getNivel() const {
    return nivel;
}

uint8_t Criatura::getAgilidad() const {
    return agilidad;
}

uint16_t Criatura::getVidaActual() const {
    return vidaActual;
}

uint16_t Criatura::getVidaMaxima() const {
    return vidaMaxima;
}

uint16_t Criatura::calcularDanio() const {
    static std::random_device randomDevice;
    static std::mt19937 generador(randomDevice());
    std::uniform_int_distribution<int> distribucion(danioMin, danioMax);

    const uint16_t danioBase = static_cast<uint16_t>(distribucion(generador));
    return static_cast<uint16_t>(fuerza) * danioBase;
}

void Criatura::recibir_danio(uint32_t danio) {
    if (vidaActual == 0) {
        return;
    }

    if (danio >= vidaActual) {
        vidaActual = 0;
    } else {
        vidaActual -= static_cast<uint16_t>(danio);
    }
}

bool Criatura::esta_muerta() const {
    return vidaActual == 0;
}

std::map<uint16_t, bool> Criatura::estaPersiguiendo() const {
    return persiguiendoJugador;
}

void Criatura::mover(Posicion nuevaPosicion) {
    posicion = nuevaPosicion;
}
