//
// Created by victoria zubieta on 29/05/2026.
//

#ifndef TALLER_TP_CLIENT_GAME_WORLD_H
#define TALLER_TP_CLIENT_GAME_WORLD_H
#include <cstdint>
#include <unordered_map>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"

// actualiza la posicion del personaje/objeto en el juego
struct EntidadRenderizable {
    uint16_t x;
    uint16_t y;
    uint8_t tipo;
    uint8_t estado;
};


class ObjectGameWorld {
private:
    std::unordered_map<uint16_t, EntidadRenderizable> entidades;
    uint16_t idCliente;
    int posX;
    int posY;
    bool isMoving;
    uint32_t lastMotionTick;

public:
    explicit ObjectGameWorld(uint16_t client_id);
    void upload_server_msg(Queue<MensajeServidor>& server_msgs, uint32_t current_tick);
    void notify_move_requested(uint32_t current_tick);

    const std::unordered_map<uint16_t, EntidadRenderizable>& entities() const;
    uint16_t client_id() const;
    int player_x() const;
    int player_y() const;
    bool player_is_moving() const;

};


#endif  // TALLER_TP_CLIENT_GAME_WORLD_H
