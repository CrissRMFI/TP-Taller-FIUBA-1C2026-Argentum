#include "receptor.h"

#include <iostream>

Receptor::Receptor(ProtocoloCliente& protocolo, Queue<MensajeServidor>* colaEntrante)
    : protocolo(protocolo),
      colaEntrante(colaEntrante) {}

void Receptor::run() {
    try {
        while (should_keep_running()) {
            MensajeServidor mensaje = protocolo.recibirMensaje();
            colaEntrante->push(mensaje);
        }
    } catch (const ClosedQueue&) {
    } catch (const std::exception& e) {
        std::cerr << "Error en receptor: " << e.what() << '\n';
    }
}