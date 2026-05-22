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
    Socket& skt_aceptador;
    Queue<ComandoJugador>& colaComandos;
    MonitorClientes& monitorClientes;
    std::vector<Cliente*> clientes;
    //uint16_t proximoId;
    std::atomic<bool> running {true};


public:
    Aceptador(Socket& skt,
              Queue<ComandoJugador>& colaComandos,
              MonitorClientes& monitorClientes);

    void run() override;
    void stop() override;
    void reap();
    void cleanup();

    ~Aceptador() override;
};

#endif
