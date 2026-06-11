#include "registro_cliente.h"

#include <iostream>

void RegistroCliente::info(const std::string& mensaje) {
    std::cout << mensaje << std::endl;
}

void RegistroCliente::error(const std::string& mensaje) {
    std::cerr << mensaje << std::endl;
}
