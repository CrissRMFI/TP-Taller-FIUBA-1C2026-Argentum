#ifndef CODIGO_ERROR_CLIENTE_H
#define CODIGO_ERROR_CLIENTE_H

#include <cstdint>

enum class CodigoErrorCliente : uint8_t {
    CONFIG_NO_LEIDA = 0,
    FUENTE_NO_CARGADA = 1,
    FONDO_CHAT_NO_CARGADO = 2,
    ITEMS_NO_LEIDOS = 3,
    ICONO_ITEM_NO_CARGADO = 4,
    PANEL_NO_CARGADO = 5,
};

#endif
