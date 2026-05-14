//
// Created by vzubieta on 5/14/26.
//

#include "client_receiver.h"
#include "../common/thread/queue.h"

ClientReceiver::ClientReceiver(ProtocoloCliente& protocol, Queue<MensajeServidor>& message_queue):
    protocol(protocol),
    queue(message_queue)
{}

void ClientReceiver::run() {
    // std::cout << "Receptor has started" << std::endl;
    try {
        while (running)
        {
            // obtener los mensajes del servidor
            MensajeServidor message = protocol.recibirMensaje();
            queue.push(message);

        }
    } catch (const std::exception&) {
        std::cout << "Receiver has stopped" << std::endl;
    }
}
void ClientReceiver::stop() { running = false; }
bool ClientReceiver::is_running() const { return running; }
