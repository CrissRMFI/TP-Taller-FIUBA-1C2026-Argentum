//
// Created by vzubieta on 5/14/26.
//

#include "client_business.h"

ClientBusiness::ClientBusiness(Queue<ComandoJugador>& incoming_data) :
        incoming_data(incoming_data) {}

void ClientBusiness::save_command(ComandoJugador action) {
    incoming_data.push(action);
}
