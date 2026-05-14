#ifndef EQUIPAMIENTO_H
#define EQUIPAMIENTO_H

#include <vector>

#include "item.h"

enum class TipoItem {
    Arma,
    Baculo,
};
class Equipamiento{
    private:
    uint16_t arma;
    uint16_t baculo;
    uint16_t defensa;
    uint16_t casco;
    uint16_t escudo;

    // Devuelve el id del item que se des-equipó, o 0 si estaba vacio el slot
    uint16_t equiparItemAtaque(uint16_t idItem);
    uint16_t equiparDefensa(uint16_t idItem);
    uint16_t equiparCasco(uint16_t idItem);
    uint16_t equiparEscudo(uint16_t idItem);
};

#endif