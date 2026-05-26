//
// Created by victoria zubieta on 22/05/2026.
//

#include "recibidor.h"

Recibidor::Recibidor(ProtocoloServidor &protocolo,
    Queue<ComandoCliente> &colaComando,
    MonitorClientes &monitor, uint16_t idCliente):
    proto(protocolo), colaComando(colaComando),
    monitor(monitor), idCliente(idCliente)
{}


void Recibidor::run() {
    std::cout << "Recibidor corriendo \n";
    try {
        while (running) {
            ComandoJugador comando = proto.recibirComando();
            std::cout << "[opcode " << static_cast<int>(comando.opcode) << "] recibido \n";
            colaComando.push(ComandoCliente{idCliente, comando});
        }
    } catch (std::exception &e) {
        std::cout << "Recibidor::run() EXCEPTION: " << e.what() << std::endl;
    }
    running = false;

    monitor.removerCliente(idCliente);
    std::cout << "[cliente " << idCliente << "] removido \n";
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
