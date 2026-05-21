#ifndef ACEPTADOR_H
#define ACEPTADOR_H

#include <cstdint>
#include <list>
#include <memory>

#include "../../common/socket/socket.h"
#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../cliente/cliente.h"
#include "../gameloop/comando_cliente.h"
#include "../gameloop/monitor_clientes.h"

class Aceptador: public Thread {
private:
    Socket skt;
    Queue<ComandoCliente>* colaComandos;
    MonitorClientes* monitorClientes;
    std::list<std::unique_ptr<Cliente>> clientes;
    uint16_t proximoId;

    void limpiarClientesMuertos();

public:
    Aceptador(const char* puerto,
              Queue<ComandoCliente>* colaComandos,
              MonitorClientes* monitorClientes);

    void run() override;
    void stop() override;

    Aceptador(const Aceptador&) = delete;
    Aceptador& operator=(const Aceptador&) = delete;
};

#endif
