#include "aceptador.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>

#include <sys/socket.h>

#include "common/socket/liberror.h"

Aceptador::Aceptador(Socket& skt,
                     Queue<ComandoCliente>& colaComandos,
                     MonitorClientes& monitorClientes,
                      Queue<EventoSesion>& colaEventos)
    : skt_aceptador(skt),
      colaComandos(colaComandos),
      monitorClientes(monitorClientes),
      colaEventos(colaEventos)
      {}

void Aceptador::run() {
    while (running) {
        try {
            Socket peer = skt_aceptador.accept();
            handshakeInicial handshake;
            auto protocolo_servidor = std::make_unique<ProtocoloServidor>(std::move(peer));
            bool conexionValida = false;
            uint16_t idCliente = 0;
            bool passwordValido = false;
            try{

                handshake = protocolo_servidor->recibirUsuario();
                auto [validPassword, id] = monitorClientes.idCliente(handshake.nombre, handshake.password);
                idCliente = id;
                passwordValido = validPassword;
                conexionValida = verificarConexionCliente(idCliente, passwordValido, handshake, *protocolo_servidor);

            }catch (const std::exception& e) {

                std::cerr << "Error al recibir procesar conexión: " << e.what() << '\n';
                continue;
            }
            
            if (!conexionValida) {
                continue;
            }
            
            std::cout << "cliente aceptado" << std::endl;
            auto *cliente = new Cliente(idCliente, std::move(protocolo_servidor), colaComandos,
                monitorClientes, colaEventos, handshake);

            // monitor asocia id_cliente con su cola de salida
            monitorClientes.agregarCliente(idCliente, cliente->obtenerColaSalida());
            cliente->start();
            clientes.push_back(cliente);

        } catch (const std::exception& e) {
            running = false;
           // std::cerr << "aceptador: " << e.what() << '\n';

        }
        reap();
    }
    cleanup();
}

bool Aceptador::verificarConexionCliente(uint16_t& idCliente, bool passwordValido, const handshakeInicial& handshake, ProtocoloServidor& protocolo_servidor) {
    if (!handshake.crearPersonaje) {
        // checkear si el cliente existe
            if (idCliente == 0) {
                protocolo_servidor.enviarEstadoUsuario(MensajeEstadoUsuario{
                        .id = idCliente,
                        .nick = handshake.nombre,
                        .error = ErrorUsuario::NombreUsuarioNoEncontrado
                });
                return false;
            } else if (!passwordValido) {
                protocolo_servidor.enviarEstadoUsuario(MensajeEstadoUsuario{
                        .id = idCliente,
                        .nick = handshake.nombre,
                        .error = ErrorUsuario::PasswordIncorrecto
                });
                return false;
            } else if (monitorClientes.estaConectado(idCliente)) {
                protocolo_servidor.enviarEstadoUsuario(MensajeEstadoUsuario{
                        .id = idCliente,
                        .nick = handshake.nombre,
                        .error = ErrorUsuario::UsuarioYaConectado
                });
                return false;
            }
            protocolo_servidor.enviarEstadoUsuario(MensajeEstadoUsuario{
                    .id = idCliente,
                    .nick = handshake.nombre,
                    .error = ErrorUsuario::Ninguno
            });     
    } else {
        // checkear si el el nick no esta en uso
        if (idCliente != 0) {
                protocolo_servidor.enviarEstadoUsuario(MensajeEstadoUsuario{
                        .id = idCliente,
                        .nick = handshake.nombre,
                        .error = ErrorUsuario::NickYaExistente
                });
                return false;
        }
        idCliente = monitorClientes.almacenarID();
        protocolo_servidor.enviarEstadoUsuario(MensajeEstadoUsuario{
                .id = idCliente,
                .nick = handshake.nombre,
                .error = ErrorUsuario::Ninguno
        });
    }
    return true;
}

void Aceptador::stop() {
    if (!running) {return;}
    running = false;

    try {

        skt_aceptador.shutdown(2);

    } catch (const LibError&) {
        // shutdown puede fallar en un socket listener; close igual debe ejecutarse.
        skt_aceptador.close();
    } catch (const std::runtime_error&) {
        // ignoramos errores al intentar destrabar accept
    }
}

void Aceptador::reap() {
    const auto new_end = std::remove_if(clientes.begin(),clientes.end(),[](auto& cliente) {
        const bool is_dead = !cliente->is_alive();
        if (is_dead) {
            cliente-> stop();
            cliente->join();
            delete cliente;
        }
        return is_dead;
    });
    clientes.erase(new_end, clientes.end());
}

void Aceptador::cleanup() {
    for (auto* cliente : clientes) {
        cliente->stop();
        cliente->join();
        delete cliente;
    }

    clientes.clear();
}

Aceptador::~Aceptador() { cleanup();}
