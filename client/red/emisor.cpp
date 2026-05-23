#include "emisor.h"

#include <iostream>

Emisor::Emisor(ProtocoloCliente& protocolo, Queue<ComandoJugador>* colaSaliente)
    : protocolo(protocolo),
      colaSaliente(colaSaliente) {}

void Emisor::run() {
    try {
        while (should_keep_running()) {
            ComandoJugador comando = colaSaliente->pop();
            protocolo.enviarComando(comando);
        }
    } catch (const ClosedQueue&) {
    } catch (const std::exception& e) {
        if (should_keep_running()) {
            std::cerr << "[emisor] " << e.what() << '\n';
        }
    }
}
