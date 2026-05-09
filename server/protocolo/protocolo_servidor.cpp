
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
        
        case Opcode::RESUCITAR:
            return recibirComandoResucitar();

        case Opcode::CURAR:
            return recibirComandoCurar();

        case Opcode::TOMAR:
            return recibirComandoTomar();
        
        case Opcode::TIRAR:
            return recibirComandoTirar();

        case Opcode::EQUIPAR:
            return recibirComandoEquipar();

        case Opcode::COMPRAR:
            return recibirComandoComprar();

        case Opcode::VENDER:
            return recibirComandoVender();


        default:
            throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::OPCODE_CLIENTE_INVALIDO));
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

ComandoJugador ProtocoloServidor::recibirComandoResucitar() {
    return ComandoJugador{
            Opcode::RESUCITAR,
            ComandoResucitar{},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoCurar() {
    uint16_t idSacerdote = recibirDosBytes();

    return ComandoJugador{
            Opcode::CURAR,
            ComandoCurar{idSacerdote},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoTomar() {
    return ComandoJugador{
            Opcode::TOMAR,
            ComandoTomar{},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoTirar() {
    uint8_t indiceItem = recibirUnByte();

    return ComandoJugador{
      Opcode::TIRAR,
      ComandoTirar{indiceItem},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoEquipar() {
    uint8_t indiceItem = recibirUnByte();

    return ComandoJugador{
      Opcode::EQUIPAR,
      ComandoEquipar{indiceItem},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoComprar() {
    uint16_t idItem = recibirDosBytes();
    uint16_t idNPC = recibirDosBytes();

    return ComandoJugador{
      Opcode::COMPRAR,
      ComandoComprar{idItem, idNPC},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoVender() {
    uint8_t indiceItem = recibirUnByte();
    uint16_t idNPC = recibirDosBytes();

    return ComandoJugador{
      Opcode::VENDER,
      ComandoVender{indiceItem, idNPC},
    };
}
