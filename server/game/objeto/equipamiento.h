#ifndef EQUIPAMIENTO_H
#define EQUIPAMIENTO_H

#include <cstdint>
#include <vector>

#include "item.h"

class Equipamiento {
  
  private:
    uint16_t arma;
    uint16_t baculo;
    uint16_t defensa;
    uint16_t casco;
    uint16_t escudo;
  
  public:
    Equipamiento();

    // Retorna los IDs des-equipados (puede ser arma+baculo por la invariante)
    std::vector<uint16_t> equiparArma(uint16_t idItem);
    std::vector<uint16_t> equiparBaculo(uint16_t idItem);
    uint16_t equiparDefensa(uint16_t idItem);
    uint16_t equiparCasco(uint16_t idItem);
    uint16_t equiparEscudo(uint16_t idItem);

    uint16_t getArma() const;
    uint16_t getBaculo() const;
    uint16_t getDefensa() const;
    uint16_t getCasco() const;
    uint16_t getEscudo() const;
};

#endif
