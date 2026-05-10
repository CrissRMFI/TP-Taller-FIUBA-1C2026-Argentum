#ifndef MONITOR_CLIENTES_H
#define MONITOR_CLIENTES_H

#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"
#include "mensaje_salida.h"

class MonitorClientes {
private:
    std::unordered_map<uint16_t, Queue<MensajeServidor>*> colasSalida;
    std::mutex mtx;

public:
    MonitorClientes() = default;

    void agregarCliente(uint16_t idCliente, Queue<MensajeServidor>* colaSalida);
    void removerCliente(uint16_t idCliente);

    void enviarA(uint16_t idCliente, const MensajeServidor& mensaje);
    void broadcast(const MensajeServidor& mensaje);

    // Con este método decidimos si el mensaje es individual o a todo el mundo
    void despachar(const MensajeSalida& mensajeSalida);

};

#endif