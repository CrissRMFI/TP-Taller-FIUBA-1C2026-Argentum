//
// Created by victoria zubieta on 29/05/2026.
//

#include "client_game_world.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <variant>

#include "../../common/mensajes/mensajes_error_accion.h"
#include "../../common/protocolo/estado_entidad.h"
#include "../audio/gestor_audio.h"

// Gracia (ms) para mantener viva la animacion de caminar entre actualizaciones
// de posicion del server. DEBE ser mayor que el tick del server (tick_ms, hoy
// 200): si es menor, la animacion se apaga antes de que llegue el proximo paso
// y se ve a tirones.
#define MOTION_GRACE_MS 260

// Porcentaje de vida por debajo del cual se avisa "vida critica".
#define UMBRAL_VIDA_BAJA 20

ObjectGameWorld::ObjectGameWorld(const uint16_t client_id):
    idCliente(client_id), posX(0), posY(0), isMoving(false), lastMotionTick(0),
    nivelAnterior(0),
    estadoAnterior(static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo)),
    vidaBajaAvisada(false) {}

int ObjectGameWorld::distanciaAlJugador(int x, int y) const {
    return std::max(std::abs(x - posX), std::abs(y - posY));
}

void ObjectGameWorld::upload_server_msg(Queue<MensajeServidor>& server_msgs,
                                            const uint32_t current_tick,
                                            GestorAudio& gestorAudio) {
    const int previous_pos_x = posX;
    const int previous_pos_y = posY;
    bool received_own_position = false;
    bool own_position_changed = false;

    MensajeServidor mensaje;
    while (server_msgs.try_pop(mensaje)) {
        if (auto* entity_position = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            const bool esNueva = entidades.find(entity_position->id) == entidades.end();
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
            } else if (esNueva && entity_position->tipo == 1) {  // criatura nueva en pantalla
                gestorAudio.reproducirEfectoPosicional(
                        "criaturaAparece",
                        distanciaAlJugador(entity_position->x, entity_position->y));
            }
        } else if (auto* entity_disappeared = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(entity_disappeared->id);
        } else if (auto* dead_entity = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            if (dead_entity->id == idCliente) {
                gestorAudio.reproducirEfecto("muereJugador");
            } else if (const auto it = entidades.find(dead_entity->id); it != entidades.end()) {
                gestorAudio.reproducirEfectoPosicional(
                        "muerte", distanciaAlJugador(it->second.x, it->second.y));
            }
            entidades.erase(dead_entity->id);
            std::cout << "[cliente] entidad muerta: " << dead_entity->id << std::endl;
        } else if (auto* estado = std::get_if<MensajeEstadoPersonaje>(&mensaje.payload)) {
            if (estado->nivel > nivelAnterior) {
                gestorAudio.reproducirEfecto("subirNivel");
            }
            const uint8_t meditando = static_cast<uint8_t>(EstadoEntidadProtocolo::Meditando);
            if (estado->estado == meditando && estadoAnterior != meditando) {
                gestorAudio.reproducirEfecto("meditar");
            }
            const bool vivo = (estado->estado == static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo));
            const bool vidaCritica = vivo && estado->vidaMax > 0 &&
                                     (estado->vidaActual * 100 / estado->vidaMax) <= UMBRAL_VIDA_BAJA;
            if (vidaCritica && !vidaBajaAvisada) {
                gestorAudio.reproducirEfecto("vidaBaja");
            }
            vidaBajaAvisada = vidaCritica;
            nivelAnterior = estado->nivel;
            estadoAnterior = estado->estado;
            std::cout << "[cliente] estado personaje: vida " << estado->vidaActual << "/"
                      << estado->vidaMax << ", mana " << estado->manaActual << "/"
                      << estado->manaMax << ", oro " << estado->oro << ", nivel "
                      << static_cast<int>(estado->nivel) << std::endl;
        } else if (auto* dano_recibido = std::get_if<MensajeDanoRecibido>(&mensaje.payload)) {
            gestorAudio.reproducirEfecto("recibirDanio");
            std::cout << "[cliente] daño recibido: " << dano_recibido->cantidad
                      << ", atacante=" << dano_recibido->idAtacante << std::endl;
        } else if (auto* dano_producido = std::get_if<MensajeDanoProducido>(&mensaje.payload)) {
            // Golpe generico (el tipo de arma/hechizo llegara con el protocolo en 2b).
            if (const auto it = entidades.find(dano_producido->idObjetivo); it != entidades.end()) {
                gestorAudio.reproducirEfectoPosicional(
                        "ataqueEspada", distanciaAlJugador(it->second.x, it->second.y));
            } else {
                gestorAudio.reproducirEfecto("ataqueEspada");
            }
            std::cout << "[cliente] daño producido: " << dano_producido->cantidad
                      << ", objetivo=" << dano_producido->idObjetivo << std::endl;
        } else if (auto* esquive = std::get_if<MensajeEsquive>(&mensaje.payload)) {
            if (const auto it = entidades.find(esquive->idEntidad); it != entidades.end()) {
                gestorAudio.reproducirEfectoPosicional(
                        "esquive", distanciaAlJugador(it->second.x, it->second.y));
            } else {
                gestorAudio.reproducirEfecto("esquive");
            }
            std::cout << "[cliente] esquive: entidad=" << esquive->idEntidad
                      << ", esquivador=" << static_cast<int>(esquive->esquivador)
                      << std::endl;
        } else if (auto* error_accion = std::get_if<MensajeErrorAccion>(&mensaje.payload)) {
            gestorAudio.reproducirEfecto("accionNoPermitida");
            std::cout << "[cliente] error de accion: " << MensajesErrorAccion::mensaje(error_accion->codigo) << std::endl;
        } else if (auto* resucitado = std::get_if<MensajeResucitado>(&mensaje.payload)) {
            posX = resucitado->x;
            posY = resucitado->y;
            lastMotionTick = current_tick;
            gestorAudio.reproducirEfecto("reviviendo");
            std::cout << "[cliente] resucitado en (" << resucitado->x << ", "
                      << resucitado->y << ")" << std::endl;
        } else if (auto* item_suelo = std::get_if<MensajeItemEnSuelo>(&mensaje.payload)) {
            gestorAudio.reproducirEfectoPosicional(
                    "itemCaeCerca", distanciaAlJugador(item_suelo->x, item_suelo->y));
            std::cout << "[cliente] item en suelo: id=" << item_suelo->idItem << ", pos=("
                      << item_suelo->x << ", " << item_suelo->y << ")" << std::endl;
        } else if (auto* oro_suelo = std::get_if<MensajeOroEnSuelo>(&mensaje.payload)) {
            gestorAudio.reproducirEfectoPosicional(
                    "itemCaeCerca", distanciaAlJugador(oro_suelo->x, oro_suelo->y));
            std::cout << "[cliente] oro en suelo: cantidad=" << oro_suelo->cantidad
                      << ", pos=(" << oro_suelo->x << ", " << oro_suelo->y << ")"
                      << std::endl;
        } else if (std::get_if<MensajeActualizarInventario>(&mensaje.payload)) {
           
            gestorAudio.reproducirEfecto("tomarItem");
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
