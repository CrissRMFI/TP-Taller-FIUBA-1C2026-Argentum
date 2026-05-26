#ifndef CLIENTE_H
#define CLIENTE_H

#include <cstdint>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/socket/socket.h"
#include "../../common/thread/queue.h"
#include "../gameloop/comando_cliente.h"
#include "../protocolo/protocolo_servidor.h"
#include "common/thread/thread.h"
#include "server/gameloop/evento_sesion.h"
#include "server/gameloop/monitor_clientes.h"

class Cliente : public Thread{
private:
    uint16_t idCliente;
    ProtocoloServidor protocolo_servidor;
    Queue<MensajeServidor> colaSalida;
    Queue<ComandoCliente>& colaComandos;
    MonitorClientes& monitorClientes;
    Queue<EventoSesion>& colaEventos;
    std::atomic<bool> estaActivo{true};

public:
    Cliente(uint16_t id,
        Socket&& skt,
        Queue<ComandoCliente>& colaComandos,
        MonitorClientes& monitor,
         Queue<EventoSesion>& colaEventos);

    uint16_t id() const;

    uint16_t obtenerId() const;
    Queue<MensajeServidor>& obtenerColaSalida();

    Cliente(const Cliente&) = delete;
    Cliente& operator=(const Cliente&) = delete;
    Cliente(Cliente&&) = delete;
    Cliente& operator=(Cliente&&) = delete;


    void run() override;
    void stop() override;

    ~Cliente() override = default;


};

#endif
