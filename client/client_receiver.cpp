//
// Created by vzubieta on 5/14/26.
//

#include "client_receiver.h"

#include "../common/socket/liberror.h"
#include "../common/thread/queue.h"


ClientReceiver::ClientReceiver(ProtocoloCliente& protocol, Queue<MensajeServidor>& message_queue):
    protocol(protocol),
    queue(message_queue)
{}

void ClientReceiver::run() {
    try {
        while (running) {
            MensajeServidor message = protocol.recibirMensaje();
            queue.push(message);
        }
    } catch (const ClosedQueue& e) {
        std::cout << "[receiver] stopped: queue closed: " << e.what() << std::endl;
    } catch (const LibError& e) {
        std::cout << "[receiver] stopped: socket/protocol error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[receiver] stopped: " << e.what() << std::endl;
    }
}
void ClientReceiver::stop() { running = false; }
bool ClientReceiver::is_running() const { return running; }
