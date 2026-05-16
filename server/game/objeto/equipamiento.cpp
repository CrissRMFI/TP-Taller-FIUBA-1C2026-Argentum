#include "equipamiento.h"

Equipamiento::Equipamiento() : arma(0), baculo(0), defensa(0), casco(0), escudo(0) {}

std::vector<uint16_t> Equipamiento::equiparArma(uint16_t idItem) {
    std::vector<uint16_t> desplazados;
    if (arma != 0)   desplazados.push_back(arma);
    if (baculo != 0) desplazados.push_back(baculo);
    arma   = idItem;
    baculo = 0;
    return desplazados;
}

std::vector<uint16_t> Equipamiento::equiparBaculo(uint16_t idItem) {
    std::vector<uint16_t> desplazados;
    if (baculo != 0) desplazados.push_back(baculo);
    if (arma != 0)   desplazados.push_back(arma);
    baculo = idItem;
    arma   = 0;
    return desplazados;
}

uint16_t Equipamiento::equiparDefensa(uint16_t idItem) {
    uint16_t anterior = defensa;
    defensa = idItem;
    return anterior;
}

uint16_t Equipamiento::equiparCasco(uint16_t idItem) {
    uint16_t anterior = casco;
    casco = idItem;
    return anterior;
}

uint16_t Equipamiento::equiparEscudo(uint16_t idItem) {
    uint16_t anterior = escudo;
    escudo = idItem;
    return anterior;
}

uint16_t Equipamiento::getArma()    const { return arma;    }
uint16_t Equipamiento::getBaculo()  const { return baculo;  }
uint16_t Equipamiento::getDefensa() const { return defensa; }
uint16_t Equipamiento::getCasco()   const { return casco;   }
uint16_t Equipamiento::getEscudo()  const { return escudo;  }
