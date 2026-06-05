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

class GestorAudio;


class ObjectGameWorld {
private:
    std::unordered_map<uint16_t, EntidadRenderizable> entidades;
    uint16_t idCliente;
    int posX;
    int posY;
    bool isMoving;
    uint32_t lastMotionTick;

    uint8_t nivelAnterior;
    uint8_t estadoAnterior;
    bool vidaBajaAvisada;

    int distanciaAlJugador(int x, int y) const;

public:
    explicit ObjectGameWorld(uint16_t client_id);
    void upload_server_msg(Queue<MensajeServidor>& server_msgs, uint32_t current_tick,
                           GestorAudio& gestorAudio);
    void notify_move_requested(uint32_t current_tick);

    const std::unordered_map<uint16_t, EntidadRenderizable>& entities() const;
    uint16_t client_id() const;
    int player_x() const;
    int player_y() const;
    bool player_is_moving() const;

};


#endif

