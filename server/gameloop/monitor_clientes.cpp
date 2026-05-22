#include "monitor_clientes.h"

#include <vector>

MonitorClientes::MonitorClientes(): proximoID(1){}


void MonitorClientes::agregarCliente(uint16_t idCliente, Queue<MensajeServidor>& colaSalida) {
    std::lock_guard<std::mutex> lock(mtx);
    colasClientes[idCliente] = &colaSalida;
}

uint16_t MonitorClientes::almacenarID() {
    std::lock_guard<std::mutex> lock(mtx);
    return proximoID++;
}
void MonitorClientes::removerCliente(uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    colasClientes.erase(idCliente);
}

Queue<MensajeServidor>* MonitorClientes::getColasClientes(uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    const auto it = colasClientes.find(idCliente);
    if (it == colasClientes.end()) {
        return nullptr;
    }
    return it->second;
}

std::string MonitorClientes::nombreCliente(uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    const auto it = nombresUsuarios.find(idCliente);
    if (it == nombresUsuarios.end()) {
        return "" ;
    }
    return it->second;
}

void MonitorClientes::setNombreCliente(const uint16_t idCliente, const std::string& nombreCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    nombresUsuarios[idCliente] = nombreCliente;
}

void MonitorClientes::enviarA(uint16_t idCliente, const MensajeServidor& mensaje) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = colasClientes.find(idCliente);
    if (it == colasClientes.end() || it->second == nullptr) {
        return;
    }

    try {
        it->second->try_push(mensaje);
    } catch (const ClosedQueue&) {
        colasClientes.erase(it);
    }
}

void MonitorClientes::broadcast(const MensajeServidor& mensaje) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<uint16_t> clientesDesconectados;

    for (auto& [idCliente, colaSalida] : colasClientes) {
        if (colaSalida == nullptr) {
            clientesDesconectados.push_back(idCliente);
            continue;
        }

        try {
            colaSalida->try_push(mensaje);
        } catch (const ClosedQueue&) {
            clientesDesconectados.push_back(idCliente);
        }
    }

    for (uint16_t idCliente : clientesDesconectados) {
        colasClientes.erase(idCliente);
    }
}

void MonitorClientes::broadcastExcepto(uint16_t idClienteExcluido, const MensajeServidor& mensaje) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<uint16_t> clientesDesconectados;

    for (auto& [idCliente, colaSalida] : colasClientes) {
        if (idCliente == idClienteExcluido) {
            continue;
        }

        if (colaSalida == nullptr) {
            clientesDesconectados.push_back(idCliente);
            continue;
        }

        try {
            colaSalida->try_push(mensaje);
        } catch (const ClosedQueue&) {
            clientesDesconectados.push_back(idCliente);
        }
    }

    for (uint16_t idCliente : clientesDesconectados) {
        colasClientes.erase(idCliente);
    }
}

void MonitorClientes::despachar(const MensajeSalida& mensajeSalida) {
    if (mensajeSalida.tipoDestino == TipoDestino::TODOS) {
        broadcast(mensajeSalida.mensaje);
    } else if (mensajeSalida.tipoDestino == TipoDestino::TODOS_EXCEPTO_UNO) {
        broadcastExcepto(mensajeSalida.idCliente, mensajeSalida.mensaje);
    } else {
        enviarA(mensajeSalida.idCliente, mensajeSalida.mensaje);
    }
}
