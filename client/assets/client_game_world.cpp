//
// Created by victoria zubieta on 29/05/2026.
//

#include "client_game_world.h"

#include <cstdlib>
#include <iostream>
#include <variant>

#include "../../common/mensajes/mensajes_error_accion.h"

// milisegundos de gracia para que la animacion sea menos brusca
#define MOTION_GRACE_MS 140

namespace {

int animation_row_for_delta(const int delta_x, const int delta_y, const int default_row) {
    // no cambios en la posicion me quedo en la fila que estaba
    if (delta_x == 0 && delta_y == 0) {
        return default_row;
    }
    // movimiento lateral --> me muevo sobre x
    if (std::abs(delta_x) >= std::abs(delta_y)) {
        return (delta_x > 0) ? 2 : 1;
    }
    // me muevo sobre y
    return (delta_y > 0) ? 0 : 3;
}

}  // namespace

ObjectGameWorld::ObjectGameWorld(const uint16_t client_id):
    idCliente(client_id), posX(0), posY(0) {}

void ObjectGameWorld::upload_server_msg(Queue<MensajeServidor>& server_msgs,
                                            const uint32_t current_tick) {
    const int previous_pos_x = posX;
    const int previous_pos_y = posY;

    MensajeServidor mensaje;
    while (server_msgs.try_pop(mensaje)) {
        if (auto* entity_position = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            const auto previous_entity = entidades.find(entity_position->id);
            const int previous_x =
                    (previous_entity != entidades.end()) ? previous_entity->second.x : entity_position->x;
            const int previous_y =
                    (previous_entity != entidades.end()) ? previous_entity->second.y : entity_position->y;
            const bool position_changed =
                    (entity_position->x != previous_x || entity_position->y != previous_y);

            entidades[entity_position->id] = EntidadRenderizable{entity_position->x,
                                                                 entity_position->y,
                                                                 entity_position->tipo,
                                                                 entity_position->estado,
                                                                 entity_position->cabeza,
                                                                 entity_position->cuerpo};

            EntityAnimationState& animation_state = animation_states[entity_position->id];
            if (position_changed) {
                animation_state.animation_row = animation_row_for_delta(
                        static_cast<int>(entity_position->x) - previous_x,
                        static_cast<int>(entity_position->y) - previous_y,
                        animation_state.animation_row);
                animation_state.last_motion_tick = current_tick;
            }
            animation_state.is_moving =
                    position_changed ||
                    (current_tick - animation_state.last_motion_tick) < MOTION_GRACE_MS;

            if (entity_position->id == idCliente) {
                posX = entity_position->x;
                posY = entity_position->y;
            }
        } else if (auto* entity_disappeared = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(entity_disappeared->id);
            animation_states.erase(entity_disappeared->id);
        } else if (auto* dead_entity = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            entidades.erase(dead_entity->id);
            animation_states.erase(dead_entity->id);
            std::cout << "[cliente] entidad muerta: " << dead_entity->id << std::endl;
        } else if (auto* estado = std::get_if<MensajeEstadoPersonaje>(&mensaje.payload)) {
            std::cout << "[cliente] estado personaje: vida " << estado->vidaActual << "/"
                      << estado->vidaMax << ", mana " << estado->manaActual << "/"
                      << estado->manaMax << ", oro " << estado->oro << ", nivel "
                      << static_cast<int>(estado->nivel) << std::endl;
        } else if (auto* dano_recibido = std::get_if<MensajeDanoRecibido>(&mensaje.payload)) {
            std::cout << "[cliente] daño recibido: " << dano_recibido->cantidad
                      << ", atacante=" << dano_recibido->idAtacante << std::endl;
        } else if (auto* dano_producido = std::get_if<MensajeDanoProducido>(&mensaje.payload)) {
            std::cout << "[cliente] daño producido: " << dano_producido->cantidad
                      << ", objetivo=" << dano_producido->idObjetivo << std::endl;
        } else if (auto* esquive = std::get_if<MensajeEsquive>(&mensaje.payload)) {
            std::cout << "[cliente] esquive: entidad=" << esquive->idEntidad
                      << ", esquivador=" << static_cast<int>(esquive->esquivador)
                      << std::endl;
        } else if (auto* error_accion = std::get_if<MensajeErrorAccion>(&mensaje.payload)) { std::cout << "[cliente] error de accion: " << MensajesErrorAccion::mensaje(error_accion->codigo) << std::endl;
        } else if (auto* resucitado = std::get_if<MensajeResucitado>(&mensaje.payload)) {
            posX = resucitado->x;
            posY = resucitado->y;
            EntityAnimationState& animation_state = animation_states[idCliente];
            animation_state.is_moving = true;
            animation_state.last_motion_tick = current_tick;
            std::cout << "[cliente] resucitado en (" << resucitado->x << ", "
                      << resucitado->y << ")" << std::endl;
        } else if (auto* item_suelo = std::get_if<MensajeItemEnSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] item en suelo: id=" << item_suelo->idItem << ", pos=("
                      << item_suelo->x << ", " << item_suelo->y << ")" << std::endl;
        } else if (auto* oro_suelo = std::get_if<MensajeOroEnSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] oro en suelo: cantidad=" << oro_suelo->cantidad
                      << ", pos=(" << oro_suelo->x << ", " << oro_suelo->y << ")"
                      << std::endl;
        } else if (auto* item_desaparecio = std::get_if<MensajeItemDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] item desaparecio del suelo: pos=("
                      << item_desaparecio->x << ", " << item_desaparecio->y << ")"
                      << std::endl;
        } else if (auto* oro_desaparecio = std::get_if<MensajeOroDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] oro desaparecio del suelo: pos=("
                      << oro_desaparecio->x << ", " << oro_desaparecio->y << ")"
                      << std::endl;
        } else if (auto* recibir_lista_items = std::get_if<MensajeListaItems>(&mensaje.payload)) {
            std::cout << "[cliente] lista items: cantidad=" << recibir_lista_items->ids.size()
                      << ", ids=[";
            for (const auto& id : recibir_lista_items->ids) {
                std::cout << id << ",";
            }
            std::cout << "]" << std::endl;
        }
    }

    for (auto& [entity_id, animation_state] : animation_states) {
        const bool client_position_changed =
                entity_id == idCliente && (posX != previous_pos_x || posY != previous_pos_y);
        if (!client_position_changed) {
            animation_state.is_moving =
                    (current_tick - animation_state.last_motion_tick) < MOTION_GRACE_MS;
        }
    }
}

void ObjectGameWorld::notify_move_requested(const uint32_t current_tick) {
    EntityAnimationState& animation_state = animation_states[idCliente];
    animation_state.is_moving = true;
    animation_state.last_motion_tick = current_tick;
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
    return entity_is_moving(idCliente);
}

bool ObjectGameWorld::entity_is_moving(const uint16_t entity_id) const {
    const auto it = animation_states.find(entity_id);
    return it != animation_states.end() && it->second.is_moving;
}

int ObjectGameWorld::entity_animation_row(const uint16_t entity_id) const {
    const auto it = animation_states.find(entity_id);
    return (it != animation_states.end()) ? it->second.animation_row : 0;
}
