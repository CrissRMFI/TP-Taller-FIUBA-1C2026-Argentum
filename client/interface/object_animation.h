//
// Created by victoria zubieta on 29/05/2026.
//

#ifndef TALLER_TP_OBJECT_ANIMATION_H
#define TALLER_TP_OBJECT_ANIMATION_H
#include <cstdint>

#include "client/client_data.h"

// animar el frame, creando la idea de movimiento

class ObjectAnimation {
private:
    int current_row = 0;
    uint8_t last_direction = 1;

public:
    void on_action(GameAction action);
    int current_animation_row() const;
    uint8_t current_direction() const;
};

#endif  // TALLER_TP_OBJECT_ANIMATION_H
