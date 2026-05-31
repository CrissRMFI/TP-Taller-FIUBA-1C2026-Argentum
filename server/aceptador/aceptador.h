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
#include "server/gameloop/evento_sesion.h"
#include "server/protocolo/protocolo_servidor.h"
#include "../../common/protocolo/dato_sesion_cliente.h"

class Aceptador: public Thread {
private:
    Socket& skt_aceptador;
    Queue<ComandoCliente>& colaComandos;
    MonitorClientes& monitorClientes;
    std::vector<Cliente*> clientes;
    Queue<EventoSesion>& colaEventos;
    //uint16_t proximoId;
    std::atomic<bool> running {true};


public:
    Aceptador(Socket& skt,
              Queue<ComandoCliente>& colaComandos,
              MonitorClientes& monitorClientes,
              Queue<EventoSesion>& colaEventos);
    
    bool verificarConexionCliente(uint16_t& idCliente,
                                  const handshakeInicial& handshake,
                                  ProtocoloServidor& protocolo_servidor);

    void run() override;
    void stop() override;
    void reap();
    void cleanup();

    ~Aceptador() override;
};

#endif
