
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
        
        
        case Opcode::DEPOSITAR_ITEM:
            return recibirComandoDepositarItem();
        case Opcode::DEPOSITAR_ORO:
            return recibirComandoDepositarOro();
        case Opcode::RETIRAR_ITEM:
            return recibirComandoRetirarItem();
        case Opcode::RETIRAR_ORO:
            return recibirComandoRetirarOro();
        case Opcode::LISTAR:
            return recibirComandoListar();
        case Opcode::CHAT_GLOBAL:
            return recibirComandoChatGlobal();
        case Opcode::CHAT_PRIVADO:
            return recibirComandoChatPrivado();


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

ComandoJugador ProtocoloServidor::recibirComandoDepositarItem() {
    uint8_t indiceItem = recibirUnByte();
    uint16_t idBanquero = recibirDosBytes();

    return ComandoJugador{
      Opcode::DEPOSITAR_ITEM,
      ComandoDepositarItem{indiceItem, idBanquero},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoDepositarOro() {
    uint32_t monto = recibirCuatroBytes();
    uint16_t idBanquero = recibirDosBytes();

    return ComandoJugador{
      Opcode::DEPOSITAR_ORO,
      ComandoDepositarOro{monto, idBanquero},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoRetirarItem() {
    uint16_t idItem = recibirDosBytes();
    uint16_t idBanquero = recibirDosBytes();

    return ComandoJugador{
      Opcode::RETIRAR_ITEM,
      ComandoRetirarItem{idItem, idBanquero},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoRetirarOro() {
    uint32_t monto = recibirCuatroBytes();
    uint16_t idBanquero = recibirDosBytes();

    return ComandoJugador{
      Opcode::RETIRAR_ORO,
      ComandoRetirarOro{monto, idBanquero},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoListar() {
    uint16_t idNPC = recibirDosBytes();

    return ComandoJugador{
      Opcode::LISTAR,
      ComandoListar{idNPC},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoChatGlobal() {
    std::string mensaje = recibirCadenaConMaximo(MAX_CHAT);

    return ComandoJugador{
      Opcode::CHAT_GLOBAL,
      ComandoChatGlobal{mensaje},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoChatPrivado() {
    std::string nickDestino = recibirCadenaConMaximo(MAX_NICK);
    std::string mensaje = recibirCadenaConMaximo(MAX_CHAT);

    return ComandoJugador{
      Opcode::CHAT_PRIVADO,
      ComandoChatPrivado{nickDestino, mensaje},
    };
}
