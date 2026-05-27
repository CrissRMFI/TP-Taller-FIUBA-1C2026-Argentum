//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"

#include <utility>

#include "client_game_loop.h"
#include "client_manager.h"
#include "../common/thread/queue.h"

Client::Client(const char* hostname, const char* port, DatosConexion& datos): skt(hostname, port), datos(datos) {
    std::cout << "[cliente] socket conectado\n" << std::endl;
}

void Client::run() {
    // Initialize SDL library
    Queue<ComandoJugador> incoming_queue;
    ClientManager manager(std::move(skt), incoming_queue, datos);
    manager.handleHandshake();
    manager.start();

    ClientGameLoop game_loop(incoming_queue, manager.get_outgoing_events());
    game_loop.init("Argentum - Parte I",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    640, 480, false);

    manager.stop();
    manager.join();
}
