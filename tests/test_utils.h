#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <gtest/gtest.h>

#include <memory>
#include <utility>

#include "../client/protocolo/protocolo_cliente.h"
#include "../common/socket/socket.h"
#include "../server/protocolo/protocolo_servidor.h"

std::pair<Socket, Socket> crearParConectado();

// Variante de ProtocoloServidor que expone enviar un byte para que los tests puedan agregar bytes manualmente

class ProtocoloServidorTest: public ProtocoloServidor {
public:
    using ProtocoloServidor::ProtocoloServidor;
    void enviarByteRaw(uint8_t b) { enviarUnByte(b); }
};

class ProtocoloFixture: public ::testing::Test {
protected:
    std::unique_ptr<ProtocoloCliente> cliente;
    std::unique_ptr<ProtocoloServidorTest> servidor;

    void SetUp() override {
        auto [skt_servidor, skt_cliente] = crearParConectado();
        cliente = std::make_unique<ProtocoloCliente>(std::move(skt_cliente));
        servidor = std::make_unique<ProtocoloServidorTest>(std::move(skt_servidor));
    }

    void TearDown() override {
        if (cliente) cliente->cerrarConexion();
        if (servidor) servidor->cerrarConexion();
    }
};

#endif
