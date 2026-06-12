#include <gtest/gtest.h>

#include <stdexcept>

#include "../common/protocolo/comando_jugador.h"
#include "../common/protocolo/opcode.h"
#include "test_utils.h"

// Validacion de Direccion

TEST_F(ProtocoloFixture, EmpezarMoverDireccionInvalidaLanzaExcepcionAlEnviar) {
    EXPECT_THROW(
        cliente->enviarComando({Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{99}}),
        std::runtime_error);
}

// Opcode invalido

TEST_F(ProtocoloFixture, OpcodeInvalidoEnMensajeServidorLanzaExcepcion) {
    servidor->enviarByteRaw(200);
    EXPECT_THROW(cliente->recibirMensaje(), std::runtime_error);
}

TEST_F(ProtocoloFixture, OpcodeDeComandoClienteRecibidoComoMensajeLanzaExcepcion) {
    servidor->enviarByteRaw(static_cast<uint8_t>(Opcode::ATACAR));
    EXPECT_THROW(cliente->recibirMensaje(), std::runtime_error);
}