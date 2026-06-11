#ifndef REGISTRO_CLIENTE_H
#define REGISTRO_CLIENTE_H

#include <string>

// Concentra la salida por consola del cliente (cout/cerr) para que el resto del
// codigo no haga I/O directo. Espejo de RegistroServidor del lado del cliente.
class RegistroCliente {
public:
    static void info(const std::string& mensaje);   // a stdout
    static void error(const std::string& mensaje);   // a stderr

    RegistroCliente() = delete;
};

#endif
