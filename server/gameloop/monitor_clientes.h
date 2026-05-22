#ifndef MONITOR_CLIENTES_H
#define MONITOR_CLIENTES_H

#include <cstdint>
#include <map>
#include <mutex>
#include <unordered_map>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"
#include "mensaje_salida.h"

class MonitorClientes {
private:
    std::unordered_map<uint16_t, Queue<MensajeServidor>*> colasClientes;
    std::map<uint16_t, std::string> nombresUsuarios;
    std::mutex mtx;
    uint16_t proximoID;

public:
    MonitorClientes();
    void agregarCliente(uint16_t idCliente, Queue<MensajeServidor>& colaSalida);
    void removerCliente(uint16_t idCliente);

    uint16_t almacenarID();
    Queue<MensajeServidor>* getColasClientes(uint16_t idCliente);

    // almacenar los nombres de los usuarios con su id
    std::string nombreCliente(uint16_t idCliente);
    void setNombreCliente(uint16_t idCliente, const std::string& nombreCliente);

    void enviarA(uint16_t idCliente, const MensajeServidor& mensaje);
    void broadcast(const MensajeServidor& mensaje);
    void broadcastExcepto(uint16_t idClienteExcluido, const MensajeServidor& mensaje);

    // Con este método decidimos si el mensaje es individual o a todo el mundo
    void despachar(const MensajeSalida& mensajeSalida);

};

#endif
