#ifndef EMISOR_H
#define EMISOR_H

#include <cstdint>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../protocolo/protocolo_servidor.h"

class Emisor: public Thread {
private:
    uint16_t idCliente;
    ProtocoloServidor& protocolo;
    Queue<MensajeServidor>* colaSalida;

public:
    Emisor(uint16_t idCliente,
           ProtocoloServidor& protocolo,
           Queue<MensajeServidor>* colaSalida);

    void run() override;

    Emisor(const Emisor&) = delete;
    Emisor& operator=(const Emisor&) = delete;
};

#endif
