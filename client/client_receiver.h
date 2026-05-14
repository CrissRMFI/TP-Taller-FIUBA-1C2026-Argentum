//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_RECEIVER_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_RECEIVER_H
#include "protocolo/protocolo_cliente.h"


class ClientReceiver : public Thread {
private:
    ProtocoloCliente& protocol;
    Queue<MensajeServidor>& queue;
    std::atomic<bool> running{true};

    public:
    ClientReceiver(ProtocoloCliente& protocolo_cliente,
        Queue<MensajeServidor>& request_queue);
    void run() override;
    void stop() override;
    bool is_running() const;
    ~ServerReceiver() override = default;

};


#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_RECEIVER_H
