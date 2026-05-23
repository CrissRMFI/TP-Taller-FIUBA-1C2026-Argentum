#include "monitor_clientes.h"

#include <memory>
#include <utility>
#include <vector>

void MonitorClientes::agregarCliente(uint16_t idCliente,
                                     std::shared_ptr<Queue<MensajeServidor>> colaSalida) {
    std::lock_guard<std::mutex> lock(mtx);

    if (colaSalida == nullptr) {
        colasSalida.erase(idCliente);
        return;
    }

    colasSalida[idCliente] = std::move(colaSalida);
}

void MonitorClientes::removerCliente(uint16_t idCliente) {
    std::lock_guard<std::mutex> lock(mtx);
    colasSalida.erase(idCliente);
}

void MonitorClientes::enviarA(uint16_t idCliente, const MensajeServidor& mensaje) {
    std::shared_ptr<Queue<MensajeServidor>> colaSalida;

    {
        std::lock_guard<std::mutex> lock(mtx);

        auto it = colasSalida.find(idCliente);
        if (it == colasSalida.end()) {
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

        auto it = colasSalida.find(idCliente);
        if (it != colasSalida.end() && it->second == colaSalida) {
            colasSalida.erase(it);
        }
    }
}

void MonitorClientes::broadcast(const MensajeServidor& mensaje) {
    std::vector<std::pair<uint16_t, std::shared_ptr<Queue<MensajeServidor>>>> snapshot;

    {
        std::lock_guard<std::mutex> lock(mtx);
        snapshot.reserve(colasSalida.size());

        for (const auto& [idCliente, colaSalida] : colasSalida) {
            snapshot.emplace_back(idCliente, colaSalida);
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

    if (clientesDesconectados.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& [idCliente, colaSalida] : clientesDesconectados) {
        auto it = colasSalida.find(idCliente);
        if (it != colasSalida.end() && it->second == colaSalida) {
            colasSalida.erase(it);
        }
    }
}

void MonitorClientes::broadcastExcepto(uint16_t idClienteExcluido, const MensajeServidor& mensaje) {
    std::vector<std::pair<uint16_t, std::shared_ptr<Queue<MensajeServidor>>>> snapshot;

    {
        std::lock_guard<std::mutex> lock(mtx);
        snapshot.reserve(colasSalida.size());

        for (const auto& [idCliente, colaSalida] : colasSalida) {
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

    if (clientesDesconectados.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& [idCliente, colaSalida] : clientesDesconectados) {
        auto it = colasSalida.find(idCliente);
        if (it != colasSalida.end() && it->second == colaSalida) {
            colasSalida.erase(it);
        }
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
