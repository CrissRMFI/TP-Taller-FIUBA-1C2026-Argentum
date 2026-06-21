//
// Created by victoria zubieta on 22/05/2026.
//

#include <string>

#include "recibidor.h"
#include "server/game/registro_servidor.h"

Recibidor::Recibidor(ProtocoloServidor& protocolo, Queue<ComandoCliente>& colaComando,
                     MonitorClientes& monitor, uint16_t idCliente) :
        proto(protocolo), colaComando(colaComando), monitor(monitor), idCliente(idCliente) {}


void Recibidor::run() {
    RegistroServidor::info("Recibidor corriendo");
    try {
        while (running) {
            ComandoJugador comando = proto.recibirComando();
            colaComando.push(ComandoCliente{idCliente, comando});
        }
    } catch (std::exception& e) {
        RegistroServidor::error(std::string("Recibidor::run() EXCEPTION: ") + e.what());
    }
    running = false;

    monitor.removerCliente(idCliente);
    RegistroServidor::info("[cliente " + std::to_string(idCliente) + "] removido");
    proto.cerrarConexion();
}

bool Recibidor::is_running() const {
    return running;
}

void Recibidor::stop() {
    running = false;
    proto.cerrarConexion();
}

// si no hay mas comandos por recibir, la idea es salir del loop
// y eliminar al cliente de la lista de regisrados
