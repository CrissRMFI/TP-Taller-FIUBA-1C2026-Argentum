#ifndef EMISOR_CLIENTE_H
#define EMISOR_CLIENTE_H

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../protocolo/protocolo_cliente.h"

class Emisor: public Thread {
private:
    ProtocoloCliente& protocolo;
    Queue<ComandoJugador>* colaSaliente;

public:
    Emisor(ProtocoloCliente& protocolo, Queue<ComandoJugador>* colaSaliente);

    void run() override;

    Emisor(const Emisor&) = delete;
    Emisor& operator=(const Emisor&) = delete;
};

#endif
