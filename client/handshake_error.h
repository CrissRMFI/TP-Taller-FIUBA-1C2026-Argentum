#ifndef HANDSHAKE_ERROR_H
#define HANDSHAKE_ERROR_H

#include <stdexcept>

#include "../common/protocolo/mensaje_servidor.h"

class HandshakeError : public std::runtime_error {
public:
    explicit HandshakeError(ErrorUsuario code) :
            std::runtime_error("handshake error"), code(code) {}

    ErrorUsuario getCode() const {
        return code;
    }

private:
    ErrorUsuario code;
};

#endif