//
// Created by victoria zubieta on 22/05/2026.
//

#include "enviador.h"

#include <numbers>
#include <string>

#include "server/game/registro_servidor.h"


Enviador::Enviador(ProtocoloServidor &proto_servidor, Queue<MensajeServidor> &colaSalida) :
    protocolo(proto_servidor),
    colaSalida(colaSalida) {}

void Enviador::run() {
    try {
        while (running) {
            MensajeServidor msj_servidor;
            msj_servidor = colaSalida.pop();
            protocolo.enviarMensaje(msj_servidor);

        }
    } catch (std::runtime_error& e) {
        RegistroServidor::error(std::string("enviador: ") + e.what());
    }
}
bool Enviador::isRunning() const {
    return running;
}

void Enviador::stop() {
    running = false;
    try {
        colaSalida.close();
    } catch (std::runtime_error& e) {
        RegistroServidor::error(std::string("enviador: cola ya cerrada ") + e.what());
    }
}
