#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <cstdint>
#include <string>
#include <vector>

#include "opcode.h"
#include "../socket/socket.h"

class Protocolo {
protected:
    Socket skt;
    bool cerrado;

    void enviarUnByte(uint8_t valor);
    void enviarDosBytes(uint16_t valor);
    void enviarCuatroBytes(uint32_t valor);
    void enviarCadenaFija(const char* cadena, uint8_t largo);
    void enviarCadenaVariable(const std::string& cadena);

    uint8_t  recibirUnByte();
    uint16_t recibirDosBytes();
    uint32_t recibirCuatroBytes();
    void     recibirCadenaFija(char* dest, uint8_t largo);
    std::string recibirCadenaVariable();

public:
    explicit Protocolo(Socket&& skt);

    bool estaCerrado() const;

    Protocolo(const Protocolo&)            = delete;
    Protocolo& operator=(const Protocolo&) = delete;

    virtual ~Protocolo() = default;
};

#endif
