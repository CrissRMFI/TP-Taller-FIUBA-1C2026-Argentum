#include "emisor.h"

#include <iostream>

Emisor::Emisor(uint16_t idCliente,
               ProtocoloServidor& protocolo,
               Queue<MensajeServidor>* colaSalida)
    : idCliente(idCliente),
      protocolo(protocolo),
      colaSalida(colaSalida) {}

void Emisor::run() {
    try {
        while (should_keep_running()) {
            MensajeServidor mensaje = colaSalida->pop();
            protocolo.enviarMensaje(mensaje);
        }
    } catch (const ClosedQueue&) {
    } catch (const std::exception& e) {
        if (should_keep_running()) {
            std::cerr << "[emisor " << idCliente << "] " << e.what() << '\n';
        }
    }
}
