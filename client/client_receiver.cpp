//
// Created by vzubieta on 5/14/26.
//

#include "client_receiver.h"

#include <string>

#include "registro_cliente.h"
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
        // La cola la cerro el cliente (cierre normal): no es una caida del servidor.
        RegistroCliente::info(std::string("[receiver] stopped: queue closed: ") + e.what());
    } catch (const LibError& e) {
        RegistroCliente::info(std::string("[receiver] stopped: socket/protocol error: ") + e.what());
        senalarCaida();
    } catch (const std::exception& e) {
        RegistroCliente::info(std::string("[receiver] stopped: ") + e.what());
        senalarCaida();
    }
}

void ClientReceiver::senalarCaida() {
    try {
        queue.close();
    } catch (const std::exception&) {
    }
}
void ClientReceiver::stop() { running = false; }
bool ClientReceiver::is_running() const { return running; }
