//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H

#include <cstdint>

#include <SDL.h>

#include "client_data.h"
#include "../common/protocolo/comando_jugador.h"
#include "../common/thread/queue.h"


class ClientBusiness {
    
public:
    explicit ClientBusiness(Queue<ComandoJugador>& incoming_data);
    void save_movement(const MovementInput& input);

private:
    Queue<ComandoJugador>& incoming_data;
};

#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
