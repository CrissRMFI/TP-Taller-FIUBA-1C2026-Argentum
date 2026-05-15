#include "aceptador.h"

#include <iostream>
#include <utility>

#include <sys/socket.h>

Aceptador::Aceptador(const char* puerto,
                     Queue<ComandoCliente>* colaComandos,
                     MonitorClientes* monitorClientes)
    : skt(puerto),
      colaComandos(colaComandos),
      monitorClientes(monitorClientes),
      proximoId(1) {}

void Aceptador::run() {
    while (should_keep_running()) {
        try {
            Socket peer = skt.accept();
            uint16_t id = proximoId++;
            auto cliente = std::make_unique<Cliente>(id, std::move(peer), colaComandos);
            cliente->iniciar();
            monitorClientes->agregarCliente(id, cliente->obtenerColaSalida());
            clientes.push_back(std::move(cliente));
            limpiarClientesMuertos();
        } catch (const std::exception& e) {
            if (should_keep_running()) {
                std::cerr << "aceptador: " << e.what() << '\n';
            }
            break;
        }
    }

    for (auto& c : clientes) {
        c->detener();
        c->esperar();
        monitorClientes->removerCliente(c->obtenerId());
    }
    clientes.clear();
}

void Aceptador::stop() {
    Thread::stop();
    try {
        skt.shutdown(SHUT_RDWR);
        skt.close();
    } catch (...) {
    }
}

void Aceptador::limpiarClientesMuertos() {
    auto it = clientes.begin();
    while (it != clientes.end()) {
        if (!(*it)->estaActivo()) {
            (*it)->esperar();
            monitorClientes->removerCliente((*it)->obtenerId());
            it = clientes.erase(it);
        } else {
            ++it;
        }
    }
}
