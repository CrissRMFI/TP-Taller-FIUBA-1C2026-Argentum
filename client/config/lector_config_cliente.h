#ifndef CLIENT_CONFIG_LECTOR_CONFIG_CLIENTE_H
#define CLIENT_CONFIG_LECTOR_CONFIG_CLIENTE_H

#include <string>

#include "config_cliente.h"


class LectorConfigCliente {
public:
    ConfigCliente cargar(const std::string& path);
};

#endif
