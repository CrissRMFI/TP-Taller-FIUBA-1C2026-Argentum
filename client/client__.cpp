//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"
#include <iostream>

#include "client_manager.h"
#include "../server/gameloop/comando_cliente.h"
#include "../common/thread/queue.h"

Client::Client(const char* hostname, const char* port): skt(hostname, port)  {}

void Client::run() {
    std::cout << "What is your name?" << std::endl;
    Queue<ComandoCliente>& incoming_queue;

    ClientManager manager(skt,incoming_queue);
    manager.run()
}