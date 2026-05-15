#include "monitor_clientes.h"

#include "../../common/thread/queue.h"

void MonitorClientes::agregarCliente(uint16_t idCliente,
                                     Queue<MensajeServidor>* colaSalida) {
    std::unique_lock<std::mutex> lock(mtx);
    colasSalida[idCliente] = colaSalida;
}

void MonitorClientes::removerCliente(uint16_t idCliente) {
    std::unique_lock<std::mutex> lock(mtx);
    colasSalida.erase(idCliente);
}

void MonitorClientes::enviarA(uint16_t idCliente, const MensajeServidor& mensaje) {
    std::unique_lock<std::mutex> lock(mtx);
    auto it = colasSalida.find(idCliente);
    if (it == colasSalida.end()) {
        return;
    }
    try {
        it->second->push(mensaje);
    } catch (const ClosedQueue&) {
    }
}

void MonitorClientes::broadcast(const MensajeServidor& mensaje) {
    std::unique_lock<std::mutex> lock(mtx);
    for (auto& [id, cola] : colasSalida) {
        try {
            cola->push(mensaje);
        } catch (const ClosedQueue&) {
        }
    }
}

void MonitorClientes::despachar(const MensajeSalida& mensajeSalida) {
    if (mensajeSalida.tipoDestino == TipoDestino::UNO) {
        enviarA(mensajeSalida.idCliente, mensajeSalida.mensaje);
    } else {
        broadcast(mensajeSalida.mensaje);
    }
}
