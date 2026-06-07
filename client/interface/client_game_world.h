//
// Created by victoria zubieta on 29/05/2026.
//

#ifndef TALLER_TP_CLIENT_GAME_WORLD_H
#define TALLER_TP_CLIENT_GAME_WORLD_H
#include <cstdint>
#include <unordered_map>

#include "client/entidad_renderizable.h"
#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"

struct EntityAnimationState {
    bool is_moving = false;
    uint32_t last_motion_tick = 0;
    int animation_row = 0;
    uint32_t move_start_tick = 0;
};


class GestorAudio;

class ObjectGameWorld {
private:
    std::unordered_map<uint16_t, EntidadRenderizable> entidades;
    // me almaceno las animaciones de las entidades, asi se visualiza correctamente el movimiento de los personajes
    std::unordered_map<uint16_t, EntityAnimationState> animation_states;
    uint16_t idCliente;
    int posX;
    int posY;
    uint8_t nivelAnterior;
    uint8_t estadoAnterior;
    bool vidaBajaAvisada;
    uint16_t vidaAnterior;
    uint16_t manaAnterior;

    int distanciaAlJugador(int x, int y) const;
    int animation_row_for_delta(int delta_x, int delta_y, int default_row);

public:
    explicit ObjectGameWorld(uint16_t client_id);
    void upload_server_msg(Queue<MensajeServidor>& server_msgs, uint32_t current_tick, GestorAudio& gestorAudio);
    void notify_move_requested(uint32_t current_tick);

    const std::unordered_map<uint16_t, EntidadRenderizable>& entities() const;
    uint16_t client_id() const;
    int player_x() const;
    int player_y() const;
    bool player_is_moving() const;
    bool entity_is_moving(uint16_t entity_id) const;
    int entity_animation_row(uint16_t entity_id) const;
};


#endif

