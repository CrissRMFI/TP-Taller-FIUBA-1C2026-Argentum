//
// Created by vzubieta on 5/14/26.
//

#include "client_sender.h"
#include "client_data.h"
#include "../common/thread/queue.h"

ClientSender::ClientSender(ProtocoloCliente& protocol, Queue<ComandoJugador>& incoming_queue):
        protocol(protocol), command_queue(incoming_queue) {}

void ClientSender::run() {
    try {
        while (running) {
            ComandoJugador comando = command_queue.pop();
            protocol.enviarComando(comando);
        }
    } catch (const ClosedQueue&) {
        std::cout << "[sender] stopped: command queue closed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[sender] stopped: " << e.what() << std::endl;
    }
}

bool ClientSender::is_running() const { return running;}

void ClientSender::stop() {
    running = false;
    try {
        command_queue.close();
    } catch (const std::runtime_error&) {
        // la cola ya estaba cerrada
    }
}
