#include "monitor_clientes.h"

#include <vector>

void MonitorClientes::agregarCliente(uint16_t idCliente, Queue<MensajeServidor>* colaSalida) {
    std::lock_guard<std::mutex> lock(mtx);
    colasSalida[idCliente] = colaSalida;
}

void MonitorClientes::removerCliente(uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    colasSalida.erase(idCliente);
}

void MonitorClientes::enviarA(uint16_t idCliente, const MensajeServidor& mensaje) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = colasSalida.find(idCliente);
    if (it == colasSalida.end() || it->second == nullptr) {
        return;
    }

    try {
        it->second->push(mensaje);
    } catch (const ClosedQueue&) {
        colasSalida.erase(it);
    }
}

void MonitorClientes::broadcast(const MensajeServidor& mensaje) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<uint16_t> clientesDesconectados;

    for (auto& [idCliente, colaSalida] : colasSalida) {
        if (colaSalida == nullptr) {
            clientesDesconectados.push_back(idCliente);
            continue;
        }

        try {
            colaSalida->push(mensaje);
        } catch (const ClosedQueue&) {
            clientesDesconectados.push_back(idCliente);
        }
    }

    for (uint16_t idCliente : clientesDesconectados) {
        colasSalida.erase(idCliente);
    }
}

void MonitorClientes::despachar(const MensajeSalida& mensajeSalida) {
    if (mensajeSalida.tipoDestino == TipoDestino::TODOS) {
        broadcast(mensajeSalida.mensaje);
    } else {
        enviarA(mensajeSalida.idCliente, mensajeSalida.mensaje);
    }
}
