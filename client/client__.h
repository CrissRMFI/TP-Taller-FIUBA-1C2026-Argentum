//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT___H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT___H

#include "client_manager.h"
#include "protocolo/protocolo_cliente.h"

class Client {
private:
    Socket skt;
public:
    explicit Client(const char* hostname, const char* port);
    void run();
};



#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT___H
