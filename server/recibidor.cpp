//
// Created by victoria zubieta on 22/05/2026.
//

#include "recibidor.h"

Recibidor::Recibidor(ProtocoloServidor &protocolo,
    Queue<ComandoJugador> &colaComando,
    MonitorClientes &monitor, uint16_t idCliente):
    proto(protocolo), colaComando(colaComando),
    monitor(monitor), idCliente(idCliente)
{}


void Recibidor::run() {
    try {
        while (running) {
            // habria que chequear que el comando recibido es valido
            // de no serlo cortar el loop

            ComandoJugador comando = proto.recibirComando();
            colaComando.push(comando);
        }
    } catch (std::exception &e) {
        std::cout << "Recibidor::run() EXCEPTION: " << e.what() << std::endl;
    }
    monitor.removerCliente(idCliente);
}

bool Recibidor::is_running() const {
    return running;
}

void Recibidor::stop() {
    running = false;
}

// si no hay mas comandos por recibir, la idea es salir del loop
// y eliminar al cliente de la lista de regisrados