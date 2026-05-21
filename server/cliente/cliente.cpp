#include "cliente.h"

#include <iostream>
#include <utility>

Cliente::Cliente(uint16_t idCliente,
                 Socket&& skt,
                 Queue<ComandoCliente>* colaComandos)
    : id(idCliente),
      protocolo(std::move(skt)),
      colaSalida(),
      colaComandos(colaComandos) {}

void Cliente::iniciar() {
    std::cout << "[cliente " << id << "] conectado\n";
}

void Cliente::detener() {}

void Cliente::esperar() {}

bool Cliente::estaActivo() const {
    return true;
}
