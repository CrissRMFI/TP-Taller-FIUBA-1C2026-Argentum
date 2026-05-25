#include "aceptador.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include <sys/socket.h>

#include "common/socket/liberror.h"

Aceptador::Aceptador(Socket& skt,
                     Queue<ComandoJugador>& colaComandos,
                     MonitorClientes& monitorClientes)
    : skt_aceptador(skt),
      colaComandos(colaComandos),
      monitorClientes(monitorClientes)
      {}

void Aceptador::run() {
    while (running) {
        try {
            Socket peer = skt_aceptador.accept();
            std::cout << "cliente aceptado" << std::endl;
            // se almacena un id por cliente
            const uint16_t clienteID = monitorClientes.almacenarID();
            auto *cliente = new Cliente (clienteID, std::move(peer), colaComandos, monitorClientes);
            // monitor asocia id_cliente con su cola de salida
            monitorClientes.agregarCliente(clienteID, cliente->obtenerColaSalida());
            cliente->start();
            clientes.push_back(cliente);

        } catch (const std::exception& e) {
            running = false;
           // std::cerr << "aceptador: " << e.what() << '\n';

        }
        reap();
    }
    cleanup();
}

void Aceptador::stop() {
    if (!running) {return;}
    running = false;

    try {

        skt_aceptador.shutdown(2);

    } catch (const LibError&) {
        // shutdown puede fallar en un socket listener; close igual debe ejecutarse.
        skt_aceptador.close();
    } catch (const std::runtime_error&) {
        // ignoramos errores al intentar destrabar accept
    }
}

void Aceptador::reap() {
    const auto new_end = std::remove_if(clientes.begin(),clientes.end(),[](auto& cliente) {
        const bool is_dead = !cliente->is_alive();
        if (is_dead) {
            cliente-> stop();
            cliente->join();
            delete cliente;
        }
        return is_dead;
    });
    clientes.erase(new_end, clientes.end());
}

void Aceptador::cleanup() {
    for (auto* cliente : clientes) {
        cliente->stop();
        cliente->join();
        delete cliente;
    }

    clientes.clear();
}

Aceptador::~Aceptador() { cleanup();}
