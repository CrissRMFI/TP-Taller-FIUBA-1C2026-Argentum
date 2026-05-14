//
// Created by vzubieta on 5/14/26.
//

#include "client_sender.h"
#include "../common/thread/queue.h"
ClientSender::ClientSender(ProtocoloCliente& protocol, Queue<ComandoJugador>& incoming_queue):
        protocol(protocol), command_queue(incoming_queue) {}

void ClientSender::run() {
    try {
        while (running) {
            // mandar los comandos al servidor
            ComandoJugador comando = command_queue.pop();
            protocol.enviarComando(comando);
        }
    } catch (const std::runtime_error&) {
        // std::cout << "Sender has stopped" << std::endl;
    }
}

bool ClientSender::is_running() const { return running;}

void ClientSender::stop() {
    running = false;
    try {
        message_queue.close();
    } catch (const std::runtime_error&) {
        // la cola ya estaba cerrada
    }
}
