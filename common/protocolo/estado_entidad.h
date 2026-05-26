#ifndef ESTADO_ENTIDAD_H
#define ESTADO_ENTIDAD_H

#include <cstdint>

enum class EstadoEntidadProtocolo : uint8_t {
    Vivo = 0,
    Fantasma = 1,
    Meditando = 2,
    Resucitando = 3,
};

#endif
