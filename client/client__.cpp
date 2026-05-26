//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"

#include <iostream>
#include <utility>

#include "client_game_loop.h"
#include "client_manager.h"
#include "pantallaLogin.h"
#include "../common/thread/queue.h"

Client::Client(const char* hostname, const char* port): skt(hostname, port) {
    std::cout << "[cliente] socket conectado\n" << std::endl;
}

void Client::run() {
    std::cout << "Bienvenido" << std::endl;

    // Initialize SDL library
    Queue<ComandoJugador> incoming_queue;
    ClientManager manager(std::move(skt), incoming_queue);
    manager.start();

    ClientGameLoop game_loop(incoming_queue);
    game_loop.init("Argentum - Parte I",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    640, 480, false);

    manager.stop();
    manager.join();
}
