//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H

#include "client_business.h"
#include "../common/thread/thread.h"
#include "../common/thread/queue.h"
#include "protocolo/protocolo_cliente.h"

class ClientManager : public Thread {
private:
    Socket skt;
    ProtocoloCliente protocol;
    ClientBusiness business;
    Queue<QueuedRequest>& incoming_data;
    Queue<MensajeServidor>& outgoing_data;
     std::atomic<bool> running{true};

public:
    ClientManager(Socket&& skt,
        Queue<ComandoJugador>& incoming_events);

    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    Queue<MensajeServidor>& get_outgoing_events();
    void run() override;
    void stop() override;
    ~ClientManager() override = default;
};
};


#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_MANAGER_H
