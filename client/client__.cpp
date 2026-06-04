//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"

#include <utility>

#include "client_game_loop.h"
#include "client_manager.h"
#include "client_business.h"
#include "config/config_cliente.h"
#include "config/lector_config_cliente.h"
#include "../common/thread/queue.h"

#ifndef CLIENT_CONFIG_PATH
#define CLIENT_CONFIG_PATH "config/client_config.toml"
#endif

Client::Client(const char* hostname, const char* port, DatosConexion& datos): skt(hostname, port), datos(datos) {
    std::cout << "[cliente] socket conectado\n" << std::endl;
}

void Client::run() {
    Queue<ComandoJugador> command_queue;
    Queue<MensajeServidor> server_message_queue;
    ClientBusiness business(command_queue);
    ClientManager manager(std::move(skt), command_queue, server_message_queue, datos);
    const uint16_t id = manager.handleHandshake();
    manager.start();

    LectorConfigCliente lector_config;
    const ConfigCliente config = lector_config.cargar(CLIENT_CONFIG_PATH);

    ClientGameLoop game_loop(server_message_queue, business, id, config);
    game_loop.init("Argentum - Parte I",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    config.ancho, config.alto, config.fullscreen);

    manager.stop();
    manager.join();
}
