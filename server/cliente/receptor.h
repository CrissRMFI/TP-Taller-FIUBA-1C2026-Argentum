#ifndef RECEPTOR_H
#define RECEPTOR_H

#include <cstdint>

#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../gameloop/comando_cliente.h"
#include "../protocolo/protocolo_servidor.h"

class Receptor: public Thread {
private:
    uint16_t idCliente;
    ProtocoloServidor& protocolo;
    Queue<ComandoCliente>* colaComandos;

public:
    Receptor(uint16_t idCliente,
             ProtocoloServidor& protocolo,
             Queue<ComandoCliente>* colaComandos);

    void run() override;

    Receptor(const Receptor&) = delete;
    Receptor& operator=(const Receptor&) = delete;
};

#endif
