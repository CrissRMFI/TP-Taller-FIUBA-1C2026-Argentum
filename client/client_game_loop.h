#ifndef TALLER_TP_CLIENT_GAME_LOOP_H
#define TALLER_TP_CLIENT_GAME_LOOP_H

#include <cstdint>
#include <optional>

#include "../common/protocolo/mensaje_servidor.h"
#include "../common/thread/queue.h"
#include "assets/client_game_world.h"
#include "assets/client_renderer.h"
#include "assets/object_animation.h"
#include "client_business.h"
#include "config/config_cliente.h"
#include "handlers/client_input_handler.h"
#include "handlers/parser_comando_chat.h"

class ClientGameLoop {
private:
    Queue<MensajeServidor>& server_messages;
    ClientInputHandler handler;
    ClientBusiness& business;
    ObjectGameWorld object_state;
    ObjectAnimation object_animation;
    ObjectRenderer object_renderer;
    ConfigCliente config;
    ParserComandoChat parser;
    bool is_running;

    void despacharComando(const ComandoJugador& command, uint32_t current_tick);
    // Interpreta una linea escrita en el mini-chat y, si es valida, la despacha.
    void procesarLineaChat(const std::string& linea, uint32_t current_tick);

public:
    ClientGameLoop(Queue<MensajeServidor>& server_messages,
                   ClientBusiness& business,
                   uint16_t idCliente,
                   const ConfigCliente& config);
    ~ClientGameLoop();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update(int it);
    void render();
    void clean();
    bool isRunning() const;
    std::optional<GameAction> animation_action_for_command(const ComandoJugador& command) const;

};


#endif
