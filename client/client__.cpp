//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"

#include <iostream>
#include <utility>

#include "client_manager.h"
#include "../common/thread/queue.h"

Client::Client(const char* hostname, const char* port): skt(hostname, port)  {}

void Client::run() {
    std::cout << "What is your name?" << std::endl;
    Queue<ComandoJugador> incoming_queue;
    ClientManager manager(std::move(skt), incoming_queue);
    manager.run();
}
