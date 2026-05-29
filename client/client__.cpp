//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"

#include <utility>

#include "client_game_loop.h"
#include "client_manager.h"
#include "client_business.h"
#include "../common/thread/queue.h"

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

    ClientGameLoop game_loop(server_message_queue, business, id);
    game_loop.init("Argentum - Parte I",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    640, 480, false);

    manager.stop();
    manager.join();
}
