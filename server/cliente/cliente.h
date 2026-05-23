#ifndef CLIENTE_H
#define CLIENTE_H

#include <cstdint>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/socket/socket.h"
#include "../../common/thread/queue.h"
#include "../gameloop/comando_cliente.h"
#include "../protocolo/protocolo_servidor.h"
#include "emisor.h"
#include "receptor.h"

class Cliente {
private:
    uint16_t id;
    ProtocoloServidor protocolo;
    Queue<MensajeServidor> colaSalida;
    Receptor receptor;
    Emisor emisor;

public:
    Cliente(uint16_t id, Socket&& skt, Queue<ComandoCliente>* colaComandos);

    Cliente(const Cliente&) = delete;
    Cliente& operator=(const Cliente&) = delete;
    Cliente(Cliente&&) = delete;
    Cliente& operator=(Cliente&&) = delete;

    void iniciar();
    void detener();
    void esperar();
    bool estaActivo() const;

    uint16_t obtenerId() const { return id; }
    Queue<MensajeServidor>* obtenerColaSalida() { return &colaSalida; }

    ~Cliente() = default;
};

#endif
