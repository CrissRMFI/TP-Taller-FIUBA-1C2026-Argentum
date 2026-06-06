#ifndef MENSAJES_ERROR_CLIENTE_H
#define MENSAJES_ERROR_CLIENTE_H

#include "codigo_error_cliente.h"

class MensajesErrorCliente {
public:
    static const char* mensaje(CodigoErrorCliente codigo);

    MensajesErrorCliente() = delete;
};

#endif
