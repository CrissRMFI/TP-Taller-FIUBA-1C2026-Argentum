#include "protocolo.h"

#include <arpa/inet.h>

#include <limits>
#include <stdexcept>
#include <utility>

Protocolo::Protocolo(Socket&& skt) : skt(std::move(skt)), cerrado(false) {}

bool Protocolo::estaCerrado() const { return cerrado; }

void Protocolo::enviarBytes(const void* datos, size_t cantidad) {
    if (cantidad == 0) {
        return;
    }

    int enviados = skt.sendall(datos, cantidad);

    if (enviados == 0) {
        cerrado = true;
        throw std::runtime_error("conexion cerrada al enviar bytes");
    }
}

void Protocolo::recibirBytes(void* destino, size_t cantidad) {
    if (cantidad == 0) {
        return;
    }

    int recibidos = skt.recvall(destino, cantidad);

    if (recibidos == 0) {
        cerrado = true;
        throw std::runtime_error("conexion cerrada al recibir bytes");
    }
}
