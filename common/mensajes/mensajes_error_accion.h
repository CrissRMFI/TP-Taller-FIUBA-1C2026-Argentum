#ifndef MENSAJES_ERROR_ACCION_H
#define MENSAJES_ERROR_ACCION_H

#include "codigo_error_accion.h"

class MensajesErrorAccion {
public:
    static const char* mensaje(CodigoErrorAccion codigo);

    MensajesErrorAccion() = delete;
};

#endif
