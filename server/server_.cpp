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
   // ConfigCompleta config_completa;

    // game loop toma poder sobre config
   // Gameloop gameloop(monitor_clientes, (std::move(config_completa)));

    Aceptador aceptador(skt, colaComandos, monitor_clientes);
    aceptador.start();
    // gameloop.start();

    char c;
    while (std::cin >> c && c != 'q') {}
    aceptador.stop();
    aceptador.join();


    // gameloop.stop();
    // gameloop.join();
}

Server::~Server() = default;

/*
 * Todavia no puedo conectar gameloop me falla porque al iniciar el juego se instancia el mapa y lanza la excepcion:
*./cmake-build-debug/taller_server 5000
 --> fatal: Las dimensiones del mapa deben ser mayores a cero
 */