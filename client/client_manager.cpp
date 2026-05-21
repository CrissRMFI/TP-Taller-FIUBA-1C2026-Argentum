//
// Created by vzubieta on 5/14/26.

#include "client_manager.h"

#include <iostream>
#include <ostream>
#include <utility>

#include "client_receiver.h"
#include "client_sender.h"
#include "../common/socket/liberror.h"
#include "../common/thread/queue.h"

ClientManager::ClientManager(Socket&& skt,
	                             Queue<ComandoJugador>& incoming_events):

	        protocol(std::move(skt)),
	        incoming_data(incoming_events)
	        {}


Queue<MensajeServidor>& ClientManager::get_outgoing_events() { return this->outgoing_data; }
void ClientManager::run() {
    ClientSender sender(protocol, incoming_data);
    ClientReceiver receiver(protocol, outgoing_data);

    receiver.start();
    sender.start();

    sender.join();
    receiver.join();
}
void ClientManager::stop() {

    running = false;
    try {
        protocol.cerrarConexion();
        incoming_data.close();
        outgoing_data.close();

    } catch (const LibError&) {
        // socket cerrado ignoramos
    } catch (const std::runtime_error&) {}
}
