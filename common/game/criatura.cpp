#include "criatura.h"

#include <algorithm>
#include <limits>

#include "aleatorio.h"

Criatura::Criatura(uint16_t idCriatura,
                   TipoCriatura tipo,
                   uint16_t vidaMaxima,
                   uint8_t nivel,
                   uint8_t fuerza,
                   uint8_t agilidad,
                   Posicion posicion,
                   uint8_t rangoAggro,
                   uint8_t danioMin,
                   uint8_t danioMax,
                   uint16_t cuerpo)
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
      cuerpo(cuerpo) {}

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

uint16_t Criatura::getCuerpo() const {
    return cuerpo;
}

uint16_t Criatura::calcularDanio(Aleatorio& aleatorio) const {
    const uint16_t danioBase = aleatorio.enteroEnRango<uint16_t>(danioMin, danioMax);
    const uint32_t danioCrudo = static_cast<uint32_t>(fuerza) * danioBase;
    return static_cast<uint16_t>(
            std::min<uint32_t>(danioCrudo, std::numeric_limits<uint16_t>::max()));
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

void Criatura::mover(Posicion nuevaPosicion) {
    posicion = nuevaPosicion;
}
