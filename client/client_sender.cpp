//
// Created by vzubieta on 5/14/26.
//

#include "client_sender.h"
#include "../common/thread/queue.h"
#include "client_data.h"
#include "registro_cliente.h"

#include <string>

ClientSender::ClientSender(ProtocoloCliente& protocol, Queue<ComandoJugador>& incoming_queue) :
        protocol(protocol), command_queue(incoming_queue) {}

void ClientSender::run() {
    try {
        while (running) {
            ComandoJugador comando = command_queue.pop();
            protocol.enviarComando(comando);
        }
    } catch (const ClosedQueue&) {
        RegistroCliente::info("[sender] stopped: command queue closed");
    } catch (const std::exception& e) {
        RegistroCliente::info(std::string("[sender] stopped: ") + e.what());
    }
}

bool ClientSender::is_running() const {
    return running;
}

void ClientSender::stop() {
    running = false;
    try {
        command_queue.close();
    } catch (const std::runtime_error&) {
        // la cola ya estaba cerrada
    }
}
