//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H

#include "../common/thread/thread.h"
#include "../common/thread/queue.h"
#include "handshake_error.h"
#include "protocolo/protocolo_cliente.h"
#include "Qt/datosConexion.h"

class ClientManager : public Thread {
private:
    ProtocoloCliente protocol;
    handshakeInicial handshake;
    Queue<ComandoJugador>& incoming_data;
    Queue<MensajeServidor> outgoing_data;
    std::atomic<bool> running{true};

public:
    ClientManager(Socket&& skt,
        Queue<ComandoJugador>& incoming_events,
        DatosConexion& datos);

    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    Queue<MensajeServidor>& get_outgoing_events();
    void run() override;
    void stop() override;

    void handleHandshake();

    ~ClientManager() override = default;
};


#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H
