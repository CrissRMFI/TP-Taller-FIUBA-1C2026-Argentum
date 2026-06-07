#ifndef CODIGO_ERROR_CLIENTE_H
#define CODIGO_ERROR_CLIENTE_H

#include <cstdint>

enum class CodigoErrorCliente : uint8_t {
    CONFIG_NO_LEIDA = 0,
    FUENTE_NO_CARGADA = 1,
    FONDO_CHAT_NO_CARGADO = 2,
};

#endif
