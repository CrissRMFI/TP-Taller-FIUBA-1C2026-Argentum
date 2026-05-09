
#include "protocolo_servidor.h"

#include <stdexcept>
#include <utility>
#include <variant>

#include "../../common/mensajes/codigo_error_accion.h"
#include "../../common/mensajes/codigo_error_protocolo.h"
#include "../../common/mensajes/mensajes_error_protocolo.h"

ProtocoloServidor::ProtocoloServidor(Socket&& skt) : Protocolo(std::move(skt)) {}

ComandoJugador ProtocoloServidor::recibirComando() {
    
    uint8_t opcode_recibido = recibirUnByte();

    Opcode opcode = (Opcode)(opcode_recibido);

    switch (opcode) {
        case Opcode::MOVER:
            return recibirComandoMover();

        case Opcode::ATACAR:
            return recibirComandoAtacar();

        case Opcode::MEDITAR:
            return recibirComandoMeditar();

        default:
            throw std::runtime_error(
                    MensajesErrorProtocolo::mensaje(
                            CodigoErrorProtocolo::OPCODE_CLIENTE_INVALIDO));
    }
}

ComandoJugador ProtocoloServidor::recibirComandoMover() {
    uint8_t direccion = recibirUnByte();
    
    return ComandoJugador{
            Opcode::MOVER,
            ComandoMover{direccion},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoAtacar() {
    uint16_t idObjetivo = recibirDosBytes();

    return ComandoJugador{
            Opcode::ATACAR,
            ComandoAtacar{idObjetivo},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoMeditar() {
    return ComandoJugador{
            Opcode::MEDITAR,
            ComandoMeditar{},
    };
}
