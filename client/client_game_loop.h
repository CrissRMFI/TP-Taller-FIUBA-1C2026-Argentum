//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_GAME_LOOP_H
#define TALLER_TP_CLIENT_GAME_LOOP_H

#include <cstdint>

#include "../common/protocolo/mensaje_servidor.h"
#include "../common/thread/queue.h"
#include "assets/client_game_world.h"
#include "assets/client_renderer.h"
#include "assets/object_animation.h"
#include "client_business.h"
#include "handlers/client_input_handler.h"

class ClientGameLoop {
private:
    Queue<MensajeServidor>& server_messages;
    ClientInputHandler handler;
    ClientBusiness& business;
    ObjectGameWorld object_state;
    ObjectAnimation object_animation;
    ObjectRenderer object_renderer;
    bool is_running;
    struct SDL_Texture* background_texture = nullptr;
    int window_width = 0;
    int window_height = 0;

public:
    ClientGameLoop(Queue<MensajeServidor>& server_messages,
                   ClientBusiness& business,
                   uint16_t idCliente);
    ~ClientGameLoop();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool isRunning() const;
    SDL_Color elegircolor(uint8_t tipo, uint8_t estado);


};


#endif
