//
// Created by vzubieta on 5/14/26.

#include "client_manager.h"

#include <iostream>
#include <ostream>
#include <utility>

#include "client_receiver.h"
#include "client_sender.h"
#include "client__.h"
#include "../common/socket/liberror.h"
#include "../common/thread/queue.h"

ClientManager::ClientManager(Socket&& skt,
                             Queue<ComandoJugador>& incoming_events):

        skt_client(std::move(skt)), /* muevo el Socket y me adueño del él */
        incoming_data(incoming_events)
        {}


Queue<MensajeServidor>& ClientManager::get_outgoing_events() { return this->outgoing_data; }
void ClientManager::run() {
    ProtocoloCliente protocol(skt);
    ClientBusiness business(incoming_data);
    business.run();
    ClientSender sender(protocol, incoming_data);
    ClientReceiver receiver(protocol, outgoing_data);
    receiver.start();
    sender.start();
    receiver.join();
    sender.stop();
    sender.join();
}
void ClientManager::stop() {

    running = false;
    try {
        /*
        skt.shutdown(2);
        skt.close();
        */
        protocol.cerrarConexion();

    } catch (const LibError&) {
        // socket cerrado ignoramos
    } catch (const std::runtime_error&) {}
}
