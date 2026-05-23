#include "receptor.h"

#include <iostream>
#include <utility>

Receptor::Receptor(uint16_t idCliente,
                   ProtocoloServidor& protocolo,
                   Queue<ComandoCliente>* colaComandos)
    : idCliente(idCliente),
      protocolo(protocolo),
      colaComandos(colaComandos) {}

void Receptor::run() {
    try {
        while (should_keep_running()) {
            ComandoJugador comando = protocolo.recibirComando();
            ComandoCliente comandoCliente{idCliente, std::move(comando)};
            colaComandos->push(comandoCliente);
        }
    } catch (const ClosedQueue&) {
    } catch (const std::exception& e) {
        if (should_keep_running()) {
            std::cerr << "[receptor " << idCliente << "] " << e.what() << '\n';
        }
    }
}
