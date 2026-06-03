//
// Created by vzubieta on 5/14/26.
//

#include "client_business.h"

ClientBusiness::ClientBusiness(Queue<ComandoJugador>& incoming_data): incoming_data(incoming_data) {}

void ClientBusiness::save_movement(const MovementInput& input) {
    if (input.start) {
        incoming_data.push(ComandoJugador{
            Opcode::EMPEZAR_MOVER,
            ComandoEmpezarMover{direction_for_protocol(input.direction)}});
    } else {
        incoming_data.push(ComandoJugador{
            Opcode::DETENER_MOVER,
            ComandoDetenerMover{}});
    }
}