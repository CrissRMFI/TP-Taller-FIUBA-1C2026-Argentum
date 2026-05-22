//
// Created by victoria zubieta on 22/05/2026.
//

#ifndef TALLER_TP_ENVIADOR_H
#define TALLER_TP_ENVIADOR_H
#include "common/thread/queue.h"
#include "common/thread/thread.h"
#include "protocolo/protocolo_servidor.h"


class Enviador: public Thread {
private:
    ProtocoloServidor& protocolo;
    Queue<MensajeServidor>& colaSalida;
    std::atomic<bool> running{true};

public:
    Enviador(ProtocoloServidor& proto_servidor, Queue<MensajeServidor>& colaSalida);
    ~Enviador() override = default;

    void run() override;
    void stop() override;
    bool isRunning() const;
};


#endif //TALLER_TP_ENVIADOR_H
