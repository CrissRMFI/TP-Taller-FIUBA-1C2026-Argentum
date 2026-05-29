//
// Created by vzubieta on 5/14/26.
//

#include "client_business.h"

ClientBusiness::ClientBusiness(Queue<ComandoJugador>& incoming_data): incoming_data(incoming_data) {}

ComandoJugador ClientBusiness::process_action(const GameAction action) const {
    return {Opcode::MOVER, ComandoMover{direction_for_protocol(action)}};
}

void ClientBusiness::save_command(const GameAction action) {
    incoming_data.push(process_action(action));
}
