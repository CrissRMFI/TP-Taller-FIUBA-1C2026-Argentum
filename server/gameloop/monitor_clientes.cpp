#include "monitor_clientes.h"

#include <memory>
#include <ranges>
#include <utility>
#include <vector>


MonitorClientes::MonitorClientes(): proximoID(1){}

uint16_t MonitorClientes::almacenarID() {
    std::lock_guard<std::mutex> lock(mtx);
    return proximoID++;
}
void MonitorClientes::agregarCliente(const uint16_t idCliente, Queue<MensajeServidor>& colaSalida) {
    std::lock_guard<std::mutex> lock(mtx);
    colasClientes[idCliente] =&colaSalida;
}

void MonitorClientes::removerCliente(const uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    const auto it = colasClientes.find(idCliente);
    if (it != colasClientes.end()) {
        // cierro cola y borro al cliente del map
        it->second->close();
        colasClientes.erase(it);
    }
    nombresUsuarios.erase(idCliente);
}

Queue<MensajeServidor>* MonitorClientes::getColasClientes(const uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    const auto it = colasClientes.find(idCliente);
    if (it == colasClientes.end()) {
        return nullptr;
    }
    return it->second;
}

std::string MonitorClientes::nombreCliente(const uint16_t idCliente) {
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
    Queue<MensajeServidor>* colaSalida;

    {
        std::lock_guard<std::mutex> lock(mtx);
        const auto it = colasClientes.find(idCliente);
        if (it == colasClientes.end()) {
            return;
        }

        colaSalida = it->second;
    }

    if (colaSalida == nullptr) {
        return;
    }

    try {
        colaSalida->try_push(mensaje);
    } catch (const ClosedQueue&) {
        std::lock_guard<std::mutex> lock(mtx);

        const auto it = colasClientes.find(idCliente);
        if (it != colasClientes.end() && it->second == colaSalida) {
            colasClientes.erase(it);
        }
    }
}

void MonitorClientes::broadcast(const MensajeServidor& mensaje) {
    std::vector<std::pair<uint16_t, Queue<MensajeServidor>*>> snapshot;

    {
        std::lock_guard<std::mutex> lock(mtx);
        snapshot.reserve(colasClientes.size());

        for (const auto& [idCliente, colaSalida] : colasClientes) {
            snapshot.emplace_back(idCliente, colaSalida);
        }
    }

    std::vector<std::pair<uint16_t, Queue<MensajeServidor>*>> clientesDesconectados;

    for (const auto& [idCliente, colaSalida] : snapshot) {
        if (colaSalida == nullptr) {
            clientesDesconectados.emplace_back(idCliente, colaSalida);
            continue;
        }

        try {
            colaSalida->try_push(mensaje);
        } catch (const ClosedQueue&) {
            clientesDesconectados.emplace_back(idCliente, colaSalida);
        }
    }

    if (clientesDesconectados.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& [idCliente, colaSalida] : clientesDesconectados) {
        if (auto it = colasClientes.find(idCliente); it != colasClientes.end() && it->second == colaSalida) {
            colasClientes.erase(it);
        }
    }
}

void MonitorClientes::broadcastExcepto(const uint16_t idClienteExcluido, const MensajeServidor& mensaje) {
    std::vector<std::pair<uint16_t, std::shared_ptr<Queue<MensajeServidor>>>> snapshot;

    {
        std::lock_guard<std::mutex> lock(mtx);
        snapshot.reserve(colasClientes.size());

        for (const auto& [idCliente, colaSalida] : colasClientes) {
            if (idCliente != idClienteExcluido) {
                snapshot.emplace_back(idCliente, colaSalida);
            }
        }
    }

    std::vector<std::pair<uint16_t, std::shared_ptr<Queue<MensajeServidor>>>> clientesDesconectados;

    for (const auto& [idCliente, colaSalida] : snapshot) {
        if (colaSalida == nullptr) {
            clientesDesconectados.emplace_back(idCliente, colaSalida);
            continue;
        }

        try {
            colaSalida->try_push(mensaje);
        } catch (const ClosedQueue&) {
            clientesDesconectados.emplace_back(idCliente, colaSalida);
        }
    }

    for (const auto &idCliente: clientesDesconectados | std::views::keys) {
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
