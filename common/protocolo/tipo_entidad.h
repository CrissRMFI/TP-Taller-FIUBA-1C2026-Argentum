#ifndef TIPO_ENTIDAD_H
#define TIPO_ENTIDAD_H

#include <cstdint>

enum class TipoEntidad : uint8_t {
    Personaje = 0,
    Criatura = 1,
    Npc = 2,
};

#endif
