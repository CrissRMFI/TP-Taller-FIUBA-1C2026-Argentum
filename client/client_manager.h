//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H

#include "../common/thread/queue.h"
#include "../common/thread/thread.h"
#include "Qt/datosConexion.h"
#include "handshake_error.h"
#include "protocolo/protocolo_cliente.h"

class ClientManager : public Thread {
private:
    ProtocoloCliente protocol;
    handshakeInicial handshake;
    Queue<ComandoJugador>& outbound_commands;
    Queue<MensajeServidor>& inbound_messages;
    std::atomic<bool> running{true};
    uint16_t idCliente;

public:
    ClientManager(Socket&& skt, Queue<ComandoJugador>& outbound_commands,
                  Queue<MensajeServidor>& inbound_messages, DatosConexion& datos);

    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    void run() override;
    void stop() override;

    uint16_t handleHandshake();

    ~ClientManager() override = default;
};


#endif  // TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H
