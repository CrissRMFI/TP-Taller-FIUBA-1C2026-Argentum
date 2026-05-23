#include "cliente.h"

#include <iostream>
#include <utility>

Cliente::Cliente(uint16_t idCliente,
                 Socket&& skt,
                 Queue<ComandoCliente>* colaComandos)
    : id(idCliente),
      protocolo(std::move(skt)),
      colaSalida(),
      receptor(idCliente, protocolo, colaComandos),
      emisor(idCliente, protocolo, &colaSalida) {}

void Cliente::iniciar() {
    std::cout << "[cliente " << id << "] conectado\n";
    receptor.start();
    emisor.start();
}

void Cliente::detener() {
    receptor.stop();
    emisor.stop();
    protocolo.cerrarConexion();
    colaSalida.close();
}

void Cliente::esperar() {
    receptor.join();
    emisor.join();
}

bool Cliente::estaActivo() const {
    return receptor.is_alive() || emisor.is_alive();
}
