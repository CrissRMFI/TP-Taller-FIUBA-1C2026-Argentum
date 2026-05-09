#ifndef MENSAJES_ERROR_PROTOCOLO_H
#define MENSAJES_ERROR_PROTOCOLO_H

#include "codigo_error_protocolo.h"

class MensajesErrorProtocolo {
public:
    static const char* mensaje(CodigoErrorProtocolo codigo);

    MensajesErrorProtocolo() = delete;
};

#endif
