//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_GAME_LOOP_H
#define TALLER_TP_CLIENT_GAME_LOOP_H

#include <memory>

#include "client_business.h"
#include "handlers/client_input_handler.h"
#include "../common/protocolo/mensaje_servidor.h"
#include "../common/thread/queue.h"
#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"

class ClientGameLoop {
private:
    Queue<ComandoJugador>& commands_queue;
    Queue<MensajeServidor>& server_messages;
    ClientInputHandler handler;
    ClientBusiness business;
    bool is_running;

public:
    ClientGameLoop(Queue<ComandoJugador>& commands_queue,
                   Queue<MensajeServidor>& server_messages);
    ~ClientGameLoop();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render(SDL2pp::Renderer& renderer);
    void clean();
    bool isRunning() const;


};


#endif //TALLER_TP_CLIENT_GAME_LOOP_H
