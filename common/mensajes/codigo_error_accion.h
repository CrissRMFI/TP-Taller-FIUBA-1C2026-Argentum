#ifndef CODIGO_ERROR_ACCION_H
#define CODIGO_ERROR_ACCION_H

#include <cstdint>

enum class CodigoErrorAccion : uint8_t {
    INVENTARIO_LLENO = 0,
    ORO_INSUFICIENTE = 1,
    NIVEL_INSUFICIENTE = 2,
    ACCION_NO_PERMITIDA = 3,
    OBJETIVO_INVALIDO = 4,
    CLAN_LLENO = 5,
};

#endif
