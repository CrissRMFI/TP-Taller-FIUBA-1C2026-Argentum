//
// Created by victoria zubieta on 29/05/2026.
//

#include "object_animation.h"

void ObjectAnimation::on_action(const GameAction action) {
    current_row = animation_row_for_action(action);
    last_direction = direction_for_protocol(action);
}

int ObjectAnimation::current_animation_row() const {
    return current_row;
}

uint8_t ObjectAnimation::current_direction() const {
    return last_direction;
}
