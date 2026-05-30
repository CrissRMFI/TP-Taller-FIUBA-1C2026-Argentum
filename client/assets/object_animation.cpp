//
// Created by victoria zubieta on 29/05/2026.
//

#include "object_animation.h"

void ObjectAnimation::on_action(const ComandoJugador& action) {
    if (action.opcode != Opcode::MOVER) {
        return;
    }

    const auto* mover = std::get_if<ComandoMover>(&action.payload);
    if (!mover) {
        return;
    }

    last_direction = mover->direccion;

    switch (mover->direccion) {
        case 0:
            current_row = 1;
            break;
        case 1:
            current_row = 0;
            break;
        case 2:
            current_row = 2;
            break;
        case 3:
            current_row = 3;
            break;
        default:
            break;
    }
}

int ObjectAnimation::current_animation_row() const {
    return current_row;
}

uint8_t ObjectAnimation::current_direction() const {
    return last_direction;
}
