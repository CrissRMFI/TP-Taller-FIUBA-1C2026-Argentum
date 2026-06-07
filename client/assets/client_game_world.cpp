#include "client_game_world.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <variant>

#include "../../common/mensajes/mensajes_error_accion.h"
#include "../../common/protocolo/estado_entidad.h"
#include "../../common/protocolo/tipo_entidad.h"
#include "../../common/protocolo/tipo_golpe.h"
#include "../audio/gestor_audio.h"

#define MOTION_GRACE_MS 260

#define UMBRAL_VIDA_BAJA 20

#define UMBRAL_CURACION 5

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
    idCliente(client_id), posX(0), posY(0),
    nivelAnterior(0),
    estadoAnterior(static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo)),
    vidaBajaAvisada(false), vidaAnterior(0), manaAnterior(0) {}

int ObjectGameWorld::distanciaAlJugador(int x, int y) const {
    return std::max(std::abs(x - posX), std::abs(y - posY));
}

void ObjectGameWorld::upload_server_msg(Queue<MensajeServidor>& server_msgs,
                                            const uint32_t current_tick,
                                            GestorAudio& gestorAudio) {
    const int previous_pos_x = posX;
    const int previous_pos_y = posY;

    MensajeServidor mensaje;
    while (server_msgs.try_pop(mensaje)) {
        if (auto* entity_position = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            const auto previous_entity = entidades.find(entity_position->id);
            const bool esNueva = (previous_entity == entidades.end());
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

            if (previous_entity == entidades.end()) {
                animation_state.previous_x = static_cast<float>(entity_position->x);
                animation_state.previous_y = static_cast<float>(entity_position->y);
                animation_state.current_x = static_cast<float>(entity_position->x);
                animation_state.current_y = static_cast<float>(entity_position->y);
                animation_state.move_start_tick = current_tick;
            }
            if (position_changed) {
                animation_state.animation_row = animation_row_for_delta(
                        static_cast<int>(entity_position->x) - previous_x,
                        static_cast<int>(entity_position->y) - previous_y,
                        animation_state.animation_row);
                animation_state.last_motion_tick = current_tick;
                animation_state.previous_x = animation_state.current_x;
                animation_state.previous_y = animation_state.current_y;
                animation_state.current_x = static_cast<float>(entity_position->x);
                animation_state.current_y = static_cast<float>(entity_position->y);
                animation_state.move_start_tick = current_tick;
            } else if (previous_entity != entidades.end()) {
                animation_state.current_x = static_cast<float>(entity_position->x);
                animation_state.current_y = static_cast<float>(entity_position->y);
            }
            animation_state.is_moving =
                    position_changed ||
                    (current_tick - animation_state.last_motion_tick) < MOTION_GRACE_MS;

            if (entity_position->id == idCliente) {
                posX = entity_position->x;
                posY = entity_position->y;
                // El sonido de pasos se maneja como loop al final, segun is_moving.
            } else if (esNueva && entity_position->tipo == 1) {  // criatura nueva en pantalla
                gestorAudio.reproducirEfectoPosicional(
                        "criaturaAparece",
                        distanciaAlJugador(entity_position->x, entity_position->y));
            }
        } else if (auto* entity_disappeared = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(entity_disappeared->id);
            animation_states.erase(entity_disappeared->id);
        } else if (auto* dead_entity = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            if (dead_entity->id == idCliente) {
                gestorAudio.reproducirEfecto("muereJugador");
            } else if (const auto it = entidades.find(dead_entity->id); it != entidades.end()) {
                const char* claveMuerte =
                        (it->second.tipo == static_cast<uint8_t>(TipoEntidad::Criatura))
                                ? "matar"
                                : "muerte";
                gestorAudio.reproducirEfectoPosicional(
                        claveMuerte, distanciaAlJugador(it->second.x, it->second.y));
            }
            entidades.erase(dead_entity->id);
            animation_states.erase(dead_entity->id);
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
            // Suba marcada de vida/mana => curacion o pocion (no la regen natural).
            if (vidaAnterior > 0 && estado->vidaActual >= vidaAnterior + UMBRAL_CURACION) {
                gestorAudio.reproducirEfecto("curarVida");
            }
            if (manaAnterior > 0 && estado->manaActual >= manaAnterior + UMBRAL_CURACION) {
                gestorAudio.reproducirEfecto("curarMana");
            }
            vidaBajaAvisada = vidaCritica;
            vidaAnterior = estado->vidaActual;
            manaAnterior = estado->manaActual;
            nivelAnterior = estado->nivel;
            estadoAnterior = estado->estado;
            std::cout << "[cliente] estado personaje: vida " << estado->vidaActual << "/"
                      << estado->vidaMax << ", mana " << estado->manaActual << "/"
                      << estado->manaMax << ", oro " << estado->oro << ", nivel "
                      << static_cast<int>(estado->nivel) << std::endl;
        } else if (auto* dano_recibido = std::get_if<MensajeDanoRecibido>(&mensaje.payload)) {
            gestorAudio.reproducirEfecto("recibirDanio");
            
            if (const auto it = entidades.find(dano_recibido->idAtacante);
                it != entidades.end() &&
                it->second.tipo == static_cast<uint8_t>(TipoEntidad::Criatura)) {
                gestorAudio.reproducirEfectoPosicional(
                        "criaturaAtacando", distanciaAlJugador(it->second.x, it->second.y));
            }
            std::cout << "[cliente] daño recibido: " << dano_recibido->cantidad
                      << ", atacante=" << dano_recibido->idAtacante << std::endl;
        } else if (auto* dano_producido = std::get_if<MensajeDanoProducido>(&mensaje.payload)) {
            // El sonido depende del arma con que se golpeo (lo decide el server).
            const char* claveAtaque = "ataqueEspada";
            switch (static_cast<TipoGolpe>(dano_producido->tipoGolpe)) {
                case TipoGolpe::Hacha:     claveAtaque = "ataqueHacha"; break;
                case TipoGolpe::Martillo:  claveAtaque = "ataqueMartillo"; break;
                case TipoGolpe::Disparo:   claveAtaque = "disparoDistancia"; break;
                case TipoGolpe::Hechizo:   claveAtaque = "lanzarHechizo"; break;
                case TipoGolpe::Explosion: claveAtaque = "explosion"; break;
                case TipoGolpe::Espada:    claveAtaque = "ataqueEspada"; break;
            }
            if (const auto it = entidades.find(dano_producido->idObjetivo); it != entidades.end()) {
                gestorAudio.reproducirEfectoPosicional(
                        claveAtaque, distanciaAlJugador(it->second.x, it->second.y));
            } else {
                gestorAudio.reproducirEfecto(claveAtaque);
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
        } else if (auto* mensaje_chat = std::get_if<MensajeChat>(&mensaje.payload)) {
            agregarLineaChat(mensaje_chat->nickOrigen + ": " + mensaje_chat->mensaje);
        } else if (auto* error_accion = std::get_if<MensajeErrorAccion>(&mensaje.payload)) {
            gestorAudio.reproducirEfecto("accionNoPermitida");
            const std::string texto = MensajesErrorAccion::mensaje(error_accion->codigo);
            agregarLineaChat("* " + texto);
        } else if (auto* resucitado = std::get_if<MensajeResucitado>(&mensaje.payload)) {
            posX = resucitado->x;
            posY = resucitado->y;
            EntityAnimationState& animation_state = animation_states[idCliente];
            animation_state.is_moving = true;
            animation_state.last_motion_tick = current_tick;
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
        } else if (auto* mensaje_clan = std::get_if<MensajeClan>(&mensaje.payload)) {
            if (mensaje_clan->tipo == TipoMensajeClan::Conectado) {
                gestorAudio.reproducirEfecto("clanMiembroEntra");
            }
        } else if (auto* item_desaparecio = std::get_if<MensajeItemDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] item desaparecio del suelo: pos=("
                      << item_desaparecio->x << ", " << item_desaparecio->y << ")"
                      << std::endl;
        } else if (auto* oro_desaparecio = std::get_if<MensajeOroDesaparecioSuelo>(&mensaje.payload)) {
            std::cout << "[cliente] oro desaparecio del suelo: pos=("
                      << oro_desaparecio->x << ", " << oro_desaparecio->y << ")"
                      << std::endl;
        } else if (auto* recibir_lista_items = std::get_if<MensajeListaItems>(&mensaje.payload)) {
            std::string linea = "Items:";
            for (const auto& id : recibir_lista_items->ids) {
                linea += " " + std::to_string(id);
            }
            agregarLineaChat(linea);
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

    
    const auto miAnimacion = animation_states.find(idCliente);
    if (miAnimacion != animation_states.end() && miAnimacion->second.is_moving) {
        gestorAudio.reproducirPasos();
    } else {
        gestorAudio.detenerPasos();
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
// devuelvo un punto medio entre la pos actual y vieja evitando movimiento brusco del personaje
InterpolatedPosition ObjectGameWorld::entity_interpolated_position(const uint16_t entity_id,
                                                                   const uint32_t current_tick) const {
    const auto state_it = animation_states.find(entity_id);
    const auto entity_it = entidades.find(entity_id);
    if (state_it == animation_states.end() || entity_it == entidades.end()) {
        return {};
    }

    const EntityAnimationState& animation_state = state_it->second;
    const float elapsed = static_cast<float>(current_tick - animation_state.move_start_tick);
    const float alpha = std::clamp(elapsed / static_cast<float>(MOTION_GRACE_MS), 0.0f, 1.0f);

    return InterpolatedPosition{
            .x = animation_state.previous_x +
                 (animation_state.current_x - animation_state.previous_x) * alpha,
            .y = animation_state.previous_y +
                 (animation_state.current_y - animation_state.previous_y) * alpha,
    };
}

void ObjectGameWorld::agregarLineaChat(const std::string& linea) {
    historialChatReciente.push_back(linea);
    while (historialChatReciente.size() > maxLineasChat) {
        historialChatReciente.pop_front();
    }
}

const std::deque<std::string>& ObjectGameWorld::historialChat() const {
    return historialChatReciente;
}

void ObjectGameWorld::setMaxLineasChat(const size_t maximo) {
    if (maximo > 0) {
        maxLineasChat = maximo;
    }
}
