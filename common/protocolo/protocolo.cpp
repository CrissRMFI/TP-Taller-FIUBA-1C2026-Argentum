#include "protocolo.h"

#include <arpa/inet.h>

#include <limits>
#include <stdexcept>
#include <utility>
#include "../mensajes/mensajes_error_protocolo.h"

Protocolo::Protocolo(Socket&& skt) : skt(std::move(skt)), cerrado(false) {}

bool Protocolo::estaCerrado() const { return cerrado; }

void Protocolo::enviarBytes(const void* datos, size_t cantidad) {
    if (cantidad == 0) {
        return;
    }

    int enviados = skt.sendall(datos, cantidad);

    if (enviados == 0) {
        cerrado = true;
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CONEXION_CERRADA_AL_ENVIAR_BYTES));
    }
}

void Protocolo::recibirBytes(void* destino, size_t cantidad) {
    if (cantidad == 0) {
        return;
    }

    int recibidos = skt.recvall(destino, cantidad);

    if (recibidos == 0) {
        cerrado = true;
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CONEXION_CERRADA_AL_RECIBIR_BYTES));
    }
}

void Protocolo::enviarUnByte(uint8_t valor) {
    enviarBytes(&valor, sizeof(valor));
}

void Protocolo::enviarDosBytes(uint16_t valor) {
    uint16_t red = htons(valor);
    enviarBytes(&red, sizeof(red));
}

void Protocolo::enviarCuatroBytes(uint32_t valor) {
    uint32_t red = htonl(valor);
    enviarBytes(&red, sizeof(red));
}

uint8_t Protocolo::recibirUnByte() {
    uint8_t valor = 0;
    recibirBytes(&valor, sizeof(valor));
    return valor;
}

uint16_t Protocolo::recibirDosBytes() {
    uint16_t valor = 0;
    recibirBytes(&valor, sizeof(valor));
    return ntohs(valor);
}

uint32_t Protocolo::recibirCuatroBytes() {
    uint32_t valor = 0;
    recibirBytes(&valor, sizeof(valor));
    return ntohl(valor);
}

void Protocolo::enviarCadena(const std::string& cadena) {
    if (cadena.size() > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CADENA_DEMASIADO_LARGA));
    }

    enviarDosBytes(static_cast<uint16_t>(cadena.size()));
    enviarBytes(cadena.data(), cadena.size());
}

void Protocolo::enviarCadenaConMaximo(const std::string& cadena, uint16_t maximo) {
    if (cadena.size() > maximo) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CADENA_DEMASIADO_LARGA));
    }

    enviarCadena(cadena);
}

std::string Protocolo::recibirCadena() {
    uint16_t largo = recibirDosBytes();

    std::string cadena(largo, '\0');

    if (largo > 0) {
        recibirBytes(&cadena[0], cadena.size());
    }

    return cadena;
}

std::string Protocolo::recibirCadenaConMaximo(uint16_t maximo) {
    uint16_t largo = recibirDosBytes();

    if (largo > maximo) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CADENA_RECIBIDA_SUPERA_MAXIMO)) ;
    }

    std::string cadena(largo, '\0');

    if (largo > 0) {
        recibirBytes(&cadena[0], cadena.size());
    }

    return cadena;
}
