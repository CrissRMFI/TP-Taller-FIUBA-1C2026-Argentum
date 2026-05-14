//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_SENDER_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_SENDER_H

#include "../common/thread/thread.h"
#include "../common/thread/queue.h"
#include "protocolo/protocolo_cliente.h"

class ClientSender : public Thread{
private:
   ProtocoloCliente& protocol;
    Queue<ComandoJugador>& command_queue;
    std::atomic<bool> running{true};

public:
    ClientSender(ServerProtocol& protocol,
        Queue<ComandoJugador>& incoming_queue);
    ~ClientSender() override = default;
    void run() override;
    void stop() override;
    bool is_running() const;

};


#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_SENDER_H
