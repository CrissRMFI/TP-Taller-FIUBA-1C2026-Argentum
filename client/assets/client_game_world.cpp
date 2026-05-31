//
// Created by victoria zubieta on 29/05/2026.
//

#include "client_game_world.h"

#include <iostream>
#include <variant>

#include "../../common/mensajes/mensajes_error_accion.h"

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
                                                                 entity_position->estado,
                                                                 entity_position->cabeza,
                                                                 entity_position->cuerpo};

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
        } else if (auto* entity_disappeared =
                           std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(entity_disappeared->id);
        } else if (auto* dead_entity = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            entidades.erase(dead_entity->id);
            std::cout << "[cliente] entidad muerta: " << dead_entity->id << std::endl;
        } else if (auto* estado = std::get_if<MensajeEstadoPersonaje>(&mensaje.payload)) {
            std::cout << "[cliente] estado personaje: vida " << estado->vidaActual << "/"
                      << estado->vidaMax << ", mana " << estado->manaActual << "/"
                      << estado->manaMax << ", oro " << estado->oro << ", nivel "
                      << static_cast<int>(estado->nivel) << std::endl;
        } else if (auto* dano_recibido = std::get_if<MensajeDanoRecibido>(&mensaje.payload)) {
            std::cout << "[cliente] daño recibido: " << dano_recibido->cantidad
                      << ", atacante=" << dano_recibido->idAtacante << std::endl;
        } else if (auto* dano_producido =
                           std::get_if<MensajeDanoProducido>(&mensaje.payload)) {
            std::cout << "[cliente] daño producido: " << dano_producido->cantidad
                      << ", objetivo=" << dano_producido->idObjetivo << std::endl;
        } else if (auto* esquive = std::get_if<MensajeEsquive>(&mensaje.payload)) {
            std::cout << "[cliente] esquive: entidad=" << esquive->idEntidad
                      << ", esquivador=" << static_cast<int>(esquive->esquivador)
                      << std::endl;
        } else if (auto* error_accion = std::get_if<MensajeErrorAccion>(&mensaje.payload)) {
            std::cout << "[cliente] error de accion: "
                      << MensajesErrorAccion::mensaje(error_accion->codigo) << std::endl;
        } else if (auto* resucitado = std::get_if<MensajeResucitado>(&mensaje.payload)) {
            posX = resucitado->x;
            posY = resucitado->y;
            lastMotionTick = current_tick;
            std::cout << "[cliente] resucitado en (" << resucitado->x << ", "
                      << resucitado->y << ")" << std::endl;
        } else if (auto* item_suelo = std::get_if<MensajeItemEnSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] item en suelo: id=" << item_suelo->idItem << ", pos=("
                      << item_suelo->x << ", " << item_suelo->y << ")" << std::endl;
        } else if (auto* oro_suelo = std::get_if<MensajeOroEnSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] oro en suelo: cantidad=" << oro_suelo->cantidad
                      << ", pos=(" << oro_suelo->x << ", " << oro_suelo->y << ")"
                      << std::endl;
        } else if (auto* item_desaparecio =
                           std::get_if<MensajeItemDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] item desaparecio del suelo: pos=("
                      << item_desaparecio->x << ", " << item_desaparecio->y << ")"
                      << std::endl;
        } else if (auto* oro_desaparecio =
                           std::get_if<MensajeOroDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] oro desaparecio del suelo: pos=("
                      << oro_desaparecio->x << ", " << oro_desaparecio->y << ")"
                      << std::endl;
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
