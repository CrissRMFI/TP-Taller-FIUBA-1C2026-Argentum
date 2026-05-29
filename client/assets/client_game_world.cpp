//
// Created by victoria zubieta on 29/05/2026.
//

#include "client_game_world.h"

#include <variant>

// milisegundos de gracia para que la animacion sea menos brusca
#define MOTION_GRACE_MS 140

ObjectGameWorld::ObjectGameWorld(const uint16_t client_id):
    idCliente(client_id), posX(0), posY(0), isMoving(false), lastMotionTick(0) {}

void ObjectGameWorld::upload_server_msg(Queue<MensajeServidor>& server_msgs,
                                            const uint32_t current_tick) {
    const int previous_pos_x = posX;
    const int previous_pos_y = posY;
    bool received_own_position = false;
    bool own_position_changed = false;

    MensajeServidor mensaje;
    while (server_msgs.try_pop(mensaje)) {
        if (auto* entity_position = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            entidades[entity_position->id] = EntidadRenderizable{entity_position->x,
                                                                 entity_position->y,
                                                                 entity_position->tipo,
                                                                 entity_position->estado};

            if (entity_position->id == idCliente) {
                received_own_position = true;
                own_position_changed =
                        (entity_position->x != posX || entity_position->y != posY);
                posX = entity_position->x;
                posY = entity_position->y;
                if (own_position_changed) {
                    lastMotionTick = current_tick;
                }
            }
        }
    }

    if (received_own_position) {
        isMoving = own_position_changed || (posX != previous_pos_x || posY != previous_pos_y);
        if (!isMoving) {
            isMoving = (current_tick - lastMotionTick) < MOTION_GRACE_MS;
        }
    } else if (isMoving) {
        isMoving = (current_tick - lastMotionTick) < MOTION_GRACE_MS;
    }
}

void ObjectGameWorld::notify_move_requested(const uint32_t current_tick) {
    isMoving = true;
    lastMotionTick = current_tick;
}

const std::unordered_map<uint16_t, EntidadRenderizable>& ObjectGameWorld::entities() const {
    return entidades;
}

uint16_t ObjectGameWorld::client_id() const {
    return idCliente;
}

int ObjectGameWorld::player_x() const {
    return posX;
}

int ObjectGameWorld::player_y() const {
    return posY;
}

bool ObjectGameWorld::player_is_moving() const {
    return isMoving;
}
