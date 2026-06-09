//
// Created by victoria zubieta on 22/05/2026.
//

#ifndef TALLER_TP_RECIBIDOR_H
#define TALLER_TP_RECIBIDOR_H
#include "common/thread/queue.h"
#include "common/thread/thread.h"
#include "gameloop/comando_cliente.h"
#include "gameloop/monitor_clientes.h"
#include "protocolo/protocolo_servidor.h"


class Recibidor: public  Thread {
private:
    ProtocoloServidor& proto;
    Queue<ComandoCliente>& colaComando;
    MonitorClientes& monitor;
    uint16_t idCliente;
    std::atomic<bool> running{true};

public:
    Recibidor(ProtocoloServidor& protocolo,
        Queue<ComandoCliente>& colaComando, MonitorClientes& monitor,
        uint16_t idCliente);

    void run() override;
    void stop() override;
    bool is_running()const;

    ~Recibidor() override = default;
};


#endif //TALLER_TP_RECIBIDOR_H
