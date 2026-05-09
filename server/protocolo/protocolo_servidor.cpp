
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

        case Opcode::FUNDAR_CLAN:
            return recibirComandoFundarClan();

        case Opcode::UNIRSE_CLAN:
            return recibirComandoUnirseClan();

        case Opcode::REVISAR_CLAN:
            return recibirComandoRevisarClan();

        case Opcode::CLAN_ACEPTAR:
            return recibirComandoClanAceptar();

        case Opcode::CLAN_RECHAZAR:
            return recibirComandoClanRechazar();

        case Opcode::CLAN_BAN:
            return recibirComandoClanBan();

        case Opcode::CLAN_KICK:
            return recibirComandoClanKick();

        case Opcode::DEJAR_CLAN:
            return recibirComandoDejarClan();


        default:
            throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::OPCODE_CLIENTE_INVALIDO));
    }
}

void ProtocoloServidor::validarDireccion(const uint8_t direccion) const {
    if (direccion > MAX_DIRECCION) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::DIRECCION_INVALIDA));
    }
}

ComandoJugador ProtocoloServidor::recibirComandoMover() {
    uint8_t direccion = recibirUnByte();

    validarDireccion(direccion);

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

ComandoJugador ProtocoloServidor::recibirComandoFundarClan() {
    std::string nombreClan = recibirCadenaConMaximo(MAX_CLAN);

    return ComandoJugador{
      Opcode::FUNDAR_CLAN,
      ComandoFundarClan{nombreClan},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoUnirseClan() {
    std::string nombreClan = recibirCadenaConMaximo(MAX_CLAN);

    return ComandoJugador{
      Opcode::UNIRSE_CLAN,
      ComandoUnirseClan{nombreClan},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoRevisarClan() {
    return ComandoJugador{
      Opcode::REVISAR_CLAN,
      ComandoRevisarClan{},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoClanAceptar() {
    std::string nick = recibirCadenaConMaximo(MAX_NICK);

    return ComandoJugador{
      Opcode::CLAN_ACEPTAR,
      ComandoGestionMiembreClan{nick},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoClanRechazar() {
    std::string nick = recibirCadenaConMaximo(MAX_NICK);

    return ComandoJugador{
      Opcode::CLAN_RECHAZAR,
      ComandoGestionMiembreClan{nick},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoClanBan() {
    std::string nick = recibirCadenaConMaximo(MAX_NICK);

    return ComandoJugador{
      Opcode::CLAN_BAN,
      ComandoGestionMiembreClan{nick},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoClanKick() {
    std::string nick = recibirCadenaConMaximo(MAX_NICK);

    return ComandoJugador{
      Opcode::CLAN_KICK,
      ComandoGestionMiembreClan{nick},
    };
}

ComandoJugador ProtocoloServidor::recibirComandoDejarClan() {
    return ComandoJugador{
      Opcode::DEJAR_CLAN,
      ComandoDejarClan{},
    };
}

void ProtocoloServidor::enviarMensaje(const MensajeServidor& mensaje) {
    switch (mensaje.opcode) {
        case Opcode::ESTADO_PERSONAJE:
            enviarEstadoPersonaje(std::get<MensajeEstadoPersonaje>(mensaje.payload));
            break;
        
        default:
            throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::OPCODE_SERVIDOR_INVALIDO));
    }
}

void ProtocoloServidor::enviarEstadoPersonaje(const MensajeEstadoPersonaje& mensaje) {
    enviarUnByte((uint8_t)(Opcode::ESTADO_PERSONAJE));

    enviarDosBytes(mensaje.vidaActual);
    enviarDosBytes(mensaje.vidaMax);
    enviarDosBytes(mensaje.manaActual);
    enviarDosBytes(mensaje.manaMax);
    enviarCuatroBytes(mensaje.oro);
    enviarUnByte(mensaje.nivel);
    enviarCuatroBytes(mensaje.experiencia);
}