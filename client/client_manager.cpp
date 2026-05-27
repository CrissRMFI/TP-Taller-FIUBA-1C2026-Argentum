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

ClientManager::ClientManager(Socket&& skt, Queue<ComandoJugador>& incoming_events, DatosConexion& datos): protocol(std::move(skt)), incoming_data(incoming_events) {
    handshake.nombre = datos.esConexionNuevoPersonaje() ? datos.getDatosNuevoPersonaje().nick : datos.getDatosPersonaje().nick;
    handshake.crearPersonaje = datos.esConexionNuevoPersonaje();
    if (datos.esConexionNuevoPersonaje()) {
        handshake.clasePersonaje = datos.getDatosNuevoPersonaje().clase;
        handshake.raza = datos.getDatosNuevoPersonaje().raza;
    }
}


Queue<MensajeServidor>& ClientManager::get_outgoing_events() { return this->outgoing_data; }
void ClientManager::run() {
    std::cout << "Conectado como: " << handshake.nombre << "\n";
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

uint16_t ClientManager::handleHandshake() {
    protocol.enviarUsuario(handshake);
    MensajeServidor mensaje = protocol.recibirEstadoUsuario();
    if (mensaje.opcode == Opcode::ESTADO_USUARIO) {
        MensajeEstadoUsuario estado = std::get<MensajeEstadoUsuario>(mensaje.payload);
        switch (estado.error) {
            case ErrorUsuario::CuentaNoEncontrada:
                throw HandshakeError(ErrorUsuario::CuentaNoEncontrada);
            case ErrorUsuario::NickYaExistente: 
                throw HandshakeError(ErrorUsuario::NickYaExistente);
            case ErrorUsuario::UsuarioYaConectado:
                throw HandshakeError(ErrorUsuario::UsuarioYaConectado);
            case ErrorUsuario::Ninguno:
                idCliente = estado.id;
                break;
            default:
                throw std::runtime_error("Error al iniciar sesión: Error desconocido\n");
        }
    } else {
        throw std::runtime_error("Error al iniciar sesión: Respuesta inesperada del servidor\n");
    }
    return idCliente;
}
