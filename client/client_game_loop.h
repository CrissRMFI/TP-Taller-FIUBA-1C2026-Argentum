//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_GAME_LOOP_H
#define TALLER_TP_CLIENT_GAME_LOOP_H

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "client_business.h"
#include "entidad_renderizable.h"
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
    std::unordered_map<uint16_t, EntidadRenderizable> entidades;
    uint16_t idCliente;
    bool is_running;
    struct SDL_Texture* background_texture = nullptr;
    int window_width = 0;
    int window_height = 0;

public:
    ClientGameLoop(Queue<ComandoJugador>& commands_queue,
                   Queue<MensajeServidor>& server_messages,
                   uint16_t idCliente);
    ~ClientGameLoop();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render(SDL2pp::Renderer& renderer);
    void clean();
    bool isRunning() const;
    SDL_Color elegircolor(uint8_t tipo, uint8_t estado);


};


#endif
