//
// Created by vzubieta on 5/14/26.
//

#include "client__.h"

#include <iostream>
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

    // El game loop se envuelve en try/catch para garantizar que, pase lo que
    // pase (incluida una excepcion del renderer), los hilos del manager se
    // detengan y se joineen. Si no, al destruirse el manager con su hilo aun
    // vivo, std::thread::~thread() llama a std::terminate() y aborta el proceso.
    ClientGameLoop game_loop(server_message_queue, business, id, config);
    game_loop.setNick(datos.esConexionNuevoPersonaje() ? datos.getDatosNuevoPersonaje().nick
                                                       : datos.getDatosPersonaje().nick);
    try {
        game_loop.init("Argentum - TALLER FIUBA",
                       SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED,
                       config.ancho, config.alto, config.fullscreen);
    } catch (const std::exception& e) {
        std::cerr << "[cliente] error en el game loop: " << e.what() << std::endl;
    }

    manager.stop();
    manager.join();
}
