#ifndef MENSAJES_ERROR_PERSISTENCIA_H
#define MENSAJES_ERROR_PERSISTENCIA_H

#include "codigo_error_persistencia.h"

class MensajesErrorPersistencia {
public:
    static const char* mensaje(CodigoErrorPersistencia codigo);
};

#endif
