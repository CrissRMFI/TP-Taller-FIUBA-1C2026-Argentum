#ifndef TALLER_TP_CLIENT_GAME_LOOP_H
#define TALLER_TP_CLIENT_GAME_LOOP_H

#include <cstdint>
#include <memory>
#include <optional>

#include "../common/protocolo/mensaje_servidor.h"
#include "../common/thread/queue.h"
#include "client_business.h"
#include "config/catalogo_items.h"
#include "config/config_cliente.h"
#include "handlers/client_input_handler.h"
#include "handlers/parser_comando_chat.h"
#include "interface/client_game_world.h"
#include "interface/object_animation.h"
#include "interface/renderer/client_renderer.h"

class GestorAudio;

class ClientGameLoop {
private:
    Queue<MensajeServidor>& server_messages;
    ClientInputHandler handler;
    ClientBusiness& business;
    ObjectGameWorld object_state;
    ObjectAnimation object_animation;
    ObjectRenderer object_renderer;
    ConfigCliente config;
    CatalogoItems catalogo;
    ParserComandoChat parser;
    std::unique_ptr<GestorAudio> gestorAudio;
    int slotInvSeleccionado = -1;
    bool pestanaHechizos = false;
    int bancoSelBoveda = -1;
    int bancoSelInv = -1;
    std::string bancoMonto;
    bool bancoMontoActivo = false;
    int tiendaSelOferta = -1;
    int tiendaSelInv = -1;
    bool is_running;
    bool servidorCaido_ = false;
    uint16_t mapaAnteriorAudio = 0;

    void despacharComando(const ComandoJugador& command, uint32_t current_tick);
    void manejarClickPanel(int x, int y);
    // Maneja los eventos (clicks + texto del monto) mientras la ventana de banco esta abierta.
    void manejarEventoBanco(const SDL_Event& event);
    // Maneja los eventos mientras el modal de comerciante/sacerdote esta abierto.
    void manejarEventoTienda(const SDL_Event& event);
    // Interpreta una linea escrita en el mini-chat y, si es valida, la despacha.
    void procesarLineaChat(const std::string& linea, uint32_t current_tick);
    void reproducirSonidoDeComando(const ComandoJugador& command);

public:
    ClientGameLoop(Queue<MensajeServidor>& server_messages, ClientBusiness& business,
                   uint16_t idCliente, const ConfigCliente& config);
    ~ClientGameLoop();

    void setNick(const std::string& nick) {
        object_state.setNick(nick);
    }
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update(int it);
    void render();
    void clean();
    bool isRunning() const;
    std::optional<GameAction> animation_action_for_command(const ComandoJugador& command) const;
};


#endif
