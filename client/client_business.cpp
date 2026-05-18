//
// Created by vzubieta on 5/14/26.
//

#include "client_business.h"

ClientBusiness::ClientBusiness(Queue<ComandoJugador>& incoming_data): incoming_data(incoming_data) {}

ComandoJugador ClientBusiness::process_action(GameAction action) const {
    switch (action) {
        case GameAction::MoveUp:
            return {Opcode::MOVER, ComandoMover{0}};

        case GameAction::MoveDown:
            return {Opcode::MOVER, ComandoMover{1}};

        case GameAction::MoveLeft:
            return {Opcode::MOVER, ComandoMover{2}};

        case GameAction::MoveRight:
            return {Opcode::MOVER, ComandoMover{3}};
    }

    throw std::runtime_error("accion de cliente invalida");
}

void ClientBusiness::save_command(GameAction action) {
    incoming_data.push(process_action(action));
}
