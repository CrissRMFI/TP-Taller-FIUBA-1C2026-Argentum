#ifndef COMMON_PROTOCOLO_TIPO_GOLPE_H
#define COMMON_PROTOCOLO_TIPO_GOLPE_H

#include <cstdint>

enum class TipoGolpe : uint8_t {
    Espada = 0,  // arma cuerpo a cuerpo por defecto
    Hacha = 1,
    Martillo = 2,
    Disparo = 3,    // arma a distancia (arco)
    Hechizo = 4,    // baculo de misil/flecha
    Explosion = 5,  // baculo de explosion
};

#endif
