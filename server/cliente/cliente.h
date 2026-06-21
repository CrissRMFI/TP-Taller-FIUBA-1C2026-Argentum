#ifndef CLIENTE_H
#define CLIENTE_H

#include <cstdint>
#include <memory>

#include "../../common/protocolo/dato_sesion_cliente.h"
#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/socket/socket.h"
#include "../../common/thread/queue.h"
#include "../gameloop/comando_cliente.h"
#include "../protocolo/protocolo_servidor.h"
#include "common/thread/thread.h"
#include "server/gameloop/evento_sesion.h"
#include "server/gameloop/monitor_clientes.h"

class Cliente : public Thread {
private:
    uint16_t idCliente;
    std::unique_ptr<ProtocoloServidor> protocolo_servidor;
    handshakeInicial dataJugador;
    Queue<MensajeServidor> colaSalida;
    Queue<ComandoCliente>& colaComandos;
    MonitorClientes& monitorClientes;
    Queue<EventoSesion>& colaEventos;
    std::atomic<bool> estaActivo{true};

public:
    Cliente(uint16_t id, std::unique_ptr<ProtocoloServidor> protocolo_servidor,
            Queue<ComandoCliente>& colaComandos, MonitorClientes& monitor,
            Queue<EventoSesion>& colaEventos, handshakeInicial handshake);

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
