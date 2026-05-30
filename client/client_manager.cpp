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
                             Queue<ComandoJugador>& outbound_commands,
                             Queue<MensajeServidor>& inbound_messages,
                             DatosConexion& datos):
        protocol(std::move(skt)),
        outbound_commands(outbound_commands),
        inbound_messages(inbound_messages) {
    handshake.nombre = datos.esConexionNuevoPersonaje() ? datos.getDatosNuevoPersonaje().nick : datos.getDatosPersonaje().nick;
    handshake.password = datos.esConexionNuevoPersonaje() ? datos.getDatosNuevoPersonaje().password : datos.getDatosPersonaje().password;
    handshake.crearPersonaje = datos.esConexionNuevoPersonaje();
    if (datos.esConexionNuevoPersonaje()) {
        handshake.clasePersonaje = datos.getDatosNuevoPersonaje().clase;
        handshake.raza = datos.getDatosNuevoPersonaje().raza;
    }
}
void ClientManager::run() {
    std::cout << "Conectado como: " << handshake.nombre << "\n";
    ClientSender sender(protocol, outbound_commands);
    ClientReceiver receiver(protocol, inbound_messages);
    receiver.start();
    sender.start();
    sender.join();
    receiver.join();
}
void ClientManager::stop() {

    running = false;
    try {
        protocol.cerrarConexion();
        outbound_commands.close();
        inbound_messages.close();

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
            case ErrorUsuario::NombreUsuarioNoEncontrado:
                throw HandshakeError(ErrorUsuario::NombreUsuarioNoEncontrado);
            case ErrorUsuario::NickYaExistente: 
                throw HandshakeError(ErrorUsuario::NickYaExistente);
            case ErrorUsuario::UsuarioYaConectado:
                throw HandshakeError(ErrorUsuario::UsuarioYaConectado);
            case ErrorUsuario::PasswordIncorrecto:
                throw HandshakeError(ErrorUsuario::PasswordIncorrecto);
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
