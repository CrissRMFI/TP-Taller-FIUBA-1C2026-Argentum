#include "server_.h"

#include <iostream>
#include <string>

#include "common/thread/queue.h"
#include "game/config/lector_config_toml.h"
#include "gameloop/gameloop.h"
#include "server/aceptador/aceptador.h"
#include "server/gameloop/comando_cliente.h"
#include "server/gameloop/monitor_clientes.h"
#include "common/persistencia/lector_mapa.h"

#ifndef SERVER_GAME_CONFIG_PATH
#define SERVER_GAME_CONFIG_PATH "config/game_config.toml"
#endif


Server::Server(const char* servname) : skt(servname) {}

void Server::run() {
    LectorConfigToml lector_config;
    ConfigCompleta config_completa = lector_config.cargar(SERVER_GAME_CONFIG_PATH);
    MonitorClientes monitor_clientes(config_completa.juego.rutaIndiceJugadores);

    LectorMapa lector_mapa;
    WorldCargado mundoCargado = lector_mapa.leerMundo(config_completa.juego.mapaArchivo, config_completa.criaturas);
    Mundo mundo(std::move(mundoCargado.mapas), std::move(mundoCargado.portales), mundoCargado.mapaPrincipalId);

    Gameloop gameloop(monitor_clientes, std::move(config_completa), std::move(mundo));
    Aceptador aceptador(skt, gameloop.getColaComandos(), monitor_clientes,
                        gameloop.getColaEventosSesion());
    aceptador.start();
    gameloop.start();

    std::string linea;
    while (std::getline(std::cin, linea) && linea != "q") {}
    aceptador.stop();
    aceptador.join();

    gameloop.detener();
    gameloop.join();
}

Server::~Server() = default;
