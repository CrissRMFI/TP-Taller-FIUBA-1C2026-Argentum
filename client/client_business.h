//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H

#include <cstdint>

#include "../common/protocolo/comando_jugador.h"
#include "../common/thread/queue.h"


class ClientBusiness {

private:
    Queue<ComandoJugador>& incoming_data;

public:
    explicit ClientBusiness(Queue<ComandoJugador>& incoming_data);
    void save_command(ComandoJugador action);
};

#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
