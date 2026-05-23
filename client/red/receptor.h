#ifndef RECEPTOR_CLIENTE_H
#define RECEPTOR_CLIENTE_H

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../protocolo/protocolo_cliente.h"

class Receptor: public Thread {
private:
    ProtocoloCliente& protocolo;
    Queue<MensajeServidor>* colaEntrante;

public:
    Receptor(ProtocoloCliente& protocolo, Queue<MensajeServidor>* colaEntrante);

    void run() override;

    Receptor(const Receptor&) = delete;
    Receptor& operator=(const Receptor&) = delete;
};

#endif
