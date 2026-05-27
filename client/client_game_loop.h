//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_GAME_LOOP_H
#define TALLER_TP_CLIENT_GAME_LOOP_H

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "client_business.h"
#include "handlers/client_input_handler.h"
#include "../common/protocolo/mensaje_servidor.h"
#include "../common/thread/queue.h"
#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"

// Snapshot local de una entidad (jugador o criatura) tal como el cliente la ve. Se actualiza al drenar mensajes del servidor en update() y se usa luego en render() para dibujarla. El cliente no decide nada sobre estos datos: solo refleja lo que el servidor dijo.
struct EntidadRenderizable {
    uint16_t x;
    uint16_t y;
    uint8_t tipo;     // ver TipoEntidad: Personaje, Criatura...
    uint8_t estado;   // ver EstadoEntidadProtocolo: Vivo, Fantasma, Meditando, Resucitando
};

class ClientGameLoop {
private:
    Queue<ComandoJugador>& commands_queue;
    Queue<MensajeServidor>& server_messages;
    ClientInputHandler handler;
    ClientBusiness business;
    std::unordered_map<uint16_t, EntidadRenderizable> entidades;
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
