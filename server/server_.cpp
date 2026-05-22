//
// Created by victoria zubieta on 22/05/2026.
//

#include "server_.h"

#include "common/thread/queue.h"
#include "gameloop/gameloop.h"
#include "server/aceptador/aceptador.h"
#include "server/gameloop/comando_cliente.h"
#include "server/gameloop/monitor_clientes.h"


Server::Server(const char* servname) : skt(servname) {}

void Server::run() {
    MonitorClientes monitor_clientes;
    Queue<ComandoJugador> colaComandos;

    //Gameloop gameloop();

    Aceptador aceptador(skt, colaComandos, monitor_clientes);
    aceptador.start();
    //gameloop.start();

    char c;
    while (std::cin >> c && c != 'q') {}
    aceptador.stop();
    aceptador.join();
    //gameloop.stop();
    //gameloop.join();
}

Server::~Server() = default;

