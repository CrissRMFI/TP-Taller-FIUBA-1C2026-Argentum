#include "protocolo_cliente.h"

#include <stdexcept>
#include <utility>
#include <variant>

#include "../../common/mensajes/codigo_error_protocolo.h"
#include "../../common/mensajes/mensajes_error_protocolo.h"

ProtocoloCliente::ProtocoloCliente(Socket&& skt) : Protocolo(std::move(skt)) {}

void ProtocoloCliente::cerrarConexion() {
    cerrado = true;
    skt.close();
}

void ProtocoloCliente::validarDireccion(uint8_t direccion) const {
    if (direccion > MAX_DIRECCION) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::DIRECCION_INVALIDA));
    }
}

void ProtocoloCliente::validarCantidad(uint16_t cantidad) const {
    if (cantidad > MAX_CANTIDAD_UINT8) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::CAMPO_INVALIDO));
    }
}

void ProtocoloCliente::enviarComando(const ComandoJugador& comando) {
    switch (comando.opcode) {
        case Opcode::MOVER:
            enviarComandoMover(std::get<ComandoMover>(comando.payload));
            break;

        case Opcode::ATACAR:
            enviarComandoAtacar(std::get<ComandoAtacar>(comando.payload));
            break;

        case Opcode::MEDITAR:
            enviarComandoMeditar(std::get<ComandoMeditar>(comando.payload));
            break;

        case Opcode::RESUCITAR:
            enviarComandoResucitar(std::get<ComandoResucitar>(comando.payload));
            break;

        case Opcode::CURAR:
            enviarComandoCurar(std::get<ComandoCurar>(comando.payload));
            break;

        case Opcode::TOMAR:
            enviarComandoTomar(std::get<ComandoTomar>(comando.payload));
            break;

        case Opcode::TIRAR:
            enviarComandoTirar(std::get<ComandoTirar>(comando.payload));
            break;

        case Opcode::EQUIPAR:
            enviarComandoEquipar(std::get<ComandoEquipar>(comando.payload));
            break;

        case Opcode::COMPRAR:
            enviarComandoComprar(std::get<ComandoComprar>(comando.payload));
            break;

        case Opcode::VENDER:
            enviarComandoVender(std::get<ComandoVender>(comando.payload));
            break;

        case Opcode::DEPOSITAR_ITEM:
            enviarComandoDepositarItem(std::get<ComandoDepositarItem>(comando.payload));
            break;

        case Opcode::DEPOSITAR_ORO:
            enviarComandoDepositarOro(std::get<ComandoDepositarOro>(comando.payload));
            break;

        case Opcode::RETIRAR_ITEM:
            enviarComandoRetirarItem(std::get<ComandoRetirarItem>(comando.payload));
            break;

        case Opcode::RETIRAR_ORO:
            enviarComandoRetirarOro(std::get<ComandoRetirarOro>(comando.payload));
            break;

        case Opcode::LISTAR:
            enviarComandoListar(std::get<ComandoListar>(comando.payload));
            break;

        case Opcode::CHAT_GLOBAL:
            enviarComandoChatGlobal(std::get<ComandoChatGlobal>(comando.payload));
            break;

        case Opcode::CHAT_PRIVADO:
            enviarComandoChatPrivado(std::get<ComandoChatPrivado>(comando.payload));
            break;

        case Opcode::FUNDAR_CLAN:
            enviarComandoFundarClan(std::get<ComandoFundarClan>(comando.payload));
            break;

        case Opcode::UNIRSE_CLAN:
            enviarComandoUnirseClan(std::get<ComandoUnirseClan>(comando.payload));
            break;

        case Opcode::REVISAR_CLAN:
            enviarComandoRevisarClan(std::get<ComandoRevisarClan>(comando.payload));
            break;

        case Opcode::CLAN_ACEPTAR:
            enviarComandoClanAceptar(std::get<ComandoGestionMiembreClan>(comando.payload));
            break;

        case Opcode::CLAN_RECHAZAR:
            enviarComandoClanRechazar(std::get<ComandoGestionMiembreClan>(comando.payload));
            break;

        case Opcode::CLAN_BAN:
            enviarComandoClanBan(std::get<ComandoGestionMiembreClan>(comando.payload));
            break;

        case Opcode::CLAN_KICK:
            enviarComandoClanKick(std::get<ComandoGestionMiembreClan>(comando.payload));
            break;

        case Opcode::DEJAR_CLAN:
            enviarComandoDejarClan(std::get<ComandoDejarClan>(comando.payload));
            break;

        default:
            throw std::runtime_error(
                    MensajesErrorProtocolo::mensaje(
                            CodigoErrorProtocolo::OPCODE_CLIENTE_INVALIDO));
    }
}

void ProtocoloCliente::enviarComandoMover(const ComandoMover& comando) {
    validarDireccion(comando.direccion);

    enviarUnByte(static_cast<uint8_t>(Opcode::MOVER));
    enviarUnByte(comando.direccion);
}

void ProtocoloCliente::enviarComandoAtacar(const ComandoAtacar& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::ATACAR));
    enviarDosBytes(comando.idObjetivo);
}

void ProtocoloCliente::enviarComandoMeditar(const ComandoMeditar&) {
    enviarUnByte(static_cast<uint8_t>(Opcode::MEDITAR));
}

void ProtocoloCliente::enviarComandoResucitar(const ComandoResucitar&) {
    enviarUnByte(static_cast<uint8_t>(Opcode::RESUCITAR));
}

void ProtocoloCliente::enviarComandoCurar(const ComandoCurar& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CURAR));
    enviarDosBytes(comando.idSacerdote);
}

void ProtocoloCliente::enviarComandoTomar(const ComandoTomar&) {
    enviarUnByte(static_cast<uint8_t>(Opcode::TOMAR));
}

void ProtocoloCliente::enviarComandoTirar(const ComandoTirar& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::TIRAR));
    enviarUnByte(comando.indiceItem);
}

void ProtocoloCliente::enviarComandoEquipar(const ComandoEquipar& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::EQUIPAR));
    enviarUnByte(comando.indiceItem);
}

void ProtocoloCliente::enviarComandoComprar(const ComandoComprar& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::COMPRAR));
    enviarDosBytes(comando.idItem);
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoVender(const ComandoVender& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::VENDER));
    enviarUnByte(comando.indiceItem);
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoDepositarItem(const ComandoDepositarItem& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::DEPOSITAR_ITEM));
    enviarUnByte(comando.indiceItem);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoDepositarOro(const ComandoDepositarOro& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::DEPOSITAR_ORO));
    enviarCuatroBytes(comando.monto);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoRetirarItem(const ComandoRetirarItem& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::RETIRAR_ITEM));
    enviarDosBytes(comando.idItem);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoRetirarOro(const ComandoRetirarOro& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::RETIRAR_ORO));
    enviarCuatroBytes(comando.monto);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoListar(const ComandoListar& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::LISTAR));
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoChatGlobal(const ComandoChatGlobal& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CHAT_GLOBAL));
    enviarCadenaConMaximo(comando.mensaje, MAX_CHAT);
}

void ProtocoloCliente::enviarComandoChatPrivado(const ComandoChatPrivado& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CHAT_PRIVADO));
    enviarCadenaConMaximo(comando.nickDestino, MAX_NICK);
    enviarCadenaConMaximo(comando.mensaje, MAX_CHAT);
}

void ProtocoloCliente::enviarComandoFundarClan(const ComandoFundarClan& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::FUNDAR_CLAN));
    enviarCadenaConMaximo(comando.nombreClan, MAX_CLAN);
}

void ProtocoloCliente::enviarComandoUnirseClan(const ComandoUnirseClan& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::UNIRSE_CLAN));
    enviarCadenaConMaximo(comando.nombreClan, MAX_CLAN);
}

void ProtocoloCliente::enviarComandoRevisarClan(const ComandoRevisarClan&) {
    enviarUnByte(static_cast<uint8_t>(Opcode::REVISAR_CLAN));
}

void ProtocoloCliente::enviarComandoClanAceptar(const ComandoGestionMiembreClan& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CLAN_ACEPTAR));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanRechazar(const ComandoGestionMiembreClan& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CLAN_RECHAZAR));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanBan(const ComandoGestionMiembreClan& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CLAN_BAN));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanKick(const ComandoGestionMiembreClan& comando) {
    enviarUnByte(static_cast<uint8_t>(Opcode::CLAN_KICK));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoDejarClan(const ComandoDejarClan&) {
    enviarUnByte(static_cast<uint8_t>(Opcode::DEJAR_CLAN));
}

//----------------------------------------------------------------


MensajeServidor ProtocoloCliente::recibirMensaje() {
    uint8_t opcodeRecibido = recibirUnByte();

    Opcode opcode = static_cast<Opcode>(opcodeRecibido);

    switch (opcode) {
        case Opcode::ESTADO_PERSONAJE:
            return recibirEstadoPersonaje();

        case Opcode::POSICION_ENTIDAD:
            return recibirPosicionEntidad();

        case Opcode::ENTIDAD_DESAPARECIO:
            return recibirEntidadDesaparecio();

        case Opcode::DANIO_RECIBIDO:
            return recibirDanoRecibido();

        case Opcode::DANIO_PRODUCIDO:
            return recibirDanoProducido();

        case Opcode::ESQUIVE:
            return recibirEsquive();

        case Opcode::MUERTE_ENTIDAD:
            return recibirMuerteEntidad();

        case Opcode::ITEM_EN_SUELO:
            return recibirItemEnSuelo();

        case Opcode::ITEM_DESAPARECIO_SUELO:
            return recibirItemDesaparecioSuelo();

        case Opcode::ACTUALIZAR_INVENTARIO:
            return recibirActualizarInventario();

        case Opcode::ACTUALIZAR_EQUIPAMIENTO:
            return recibirActualizarEquipamiento();

        case Opcode::MENSAJE_CHAT:
            return recibirMensajeChat();

        case Opcode::MENSAJE_CLAN:
            return recibirMensajeClan();

        case Opcode::RESUCITADO:
            return recibirResucitado();

        case Opcode::LISTA_ITEMS:
            return recibirListaItems();

        case Opcode::ERROR_ACCION:
            return recibirErrorAccion();

        default:
            throw std::runtime_error(
                    MensajesErrorProtocolo::mensaje(
                            CodigoErrorProtocolo::OPCODE_SERVIDOR_INVALIDO));
    }
}

//----------------------------------------------------------------

MensajeServidor ProtocoloCliente::recibirEstadoPersonaje() {
    uint16_t vidaActual = recibirDosBytes();
    uint16_t vidaMax = recibirDosBytes();
    uint16_t manaActual = recibirDosBytes();
    uint16_t manaMax = recibirDosBytes();
    uint32_t oro = recibirCuatroBytes();
    uint8_t nivel = recibirUnByte();
    uint32_t experiencia = recibirCuatroBytes();

    return MensajeServidor{
            Opcode::ESTADO_PERSONAJE,
            MensajeEstadoPersonaje{
                    vidaActual,
                    vidaMax,
                    manaActual,
                    manaMax,
                    oro,
                    nivel,
                    experiencia,
            },
    };
}

MensajeServidor ProtocoloCliente::recibirPosicionEntidad() {
    uint16_t id = recibirDosBytes();
    uint16_t x = recibirDosBytes();
    uint16_t y = recibirDosBytes();
    uint8_t tipo = recibirUnByte();
    uint8_t estado = recibirUnByte();

    validarTipoEntidad(tipo);
    validarEstadoEntidad(estado);

    return MensajeServidor{
            Opcode::POSICION_ENTIDAD,
            MensajePosicionEntidad{id, x, y, tipo, estado},
    };
}

MensajeServidor ProtocoloCliente::recibirEntidadDesaparecio() {
    uint16_t id = recibirDosBytes();

    return MensajeServidor{
            Opcode::ENTIDAD_DESAPARECIO,
            MensajeEntidadDesaparecio{id},
    };
}

MensajeServidor ProtocoloCliente::recibirDanoRecibido() {
    uint16_t cantidad = recibirDosBytes();
    uint16_t idAtacante = recibirDosBytes();

    return MensajeServidor{
            Opcode::DANIO_RECIBIDO,
            MensajeDanoRecibido{cantidad, idAtacante},
    };
}

MensajeServidor ProtocoloCliente::recibirDanoProducido() {
    uint16_t cantidad = recibirDosBytes();
    uint16_t idObjetivo = recibirDosBytes();

    return MensajeServidor{
            Opcode::DANIO_PRODUCIDO,
            MensajeDanoProducido{cantidad, idObjetivo},
    };
}

MensajeServidor ProtocoloCliente::recibirEsquive() {
    uint16_t idEntidad = recibirDosBytes();
    uint8_t esquivador = recibirUnByte();

    validarEsquivador(esquivador);

    return MensajeServidor{
            Opcode::ESQUIVE,
            MensajeEsquive{idEntidad, esquivador},
    };
}

MensajeServidor ProtocoloCliente::recibirMuerteEntidad() {
    uint16_t id = recibirDosBytes();

    return MensajeServidor{
            Opcode::MUERTE_ENTIDAD,
            MensajeMuerteEntidad{id},
    };
}

MensajeServidor ProtocoloCliente::recibirItemEnSuelo() {
    uint16_t idItem = recibirDosBytes();
    uint16_t x = recibirDosBytes();
    uint16_t y = recibirDosBytes();

    return MensajeServidor{
            Opcode::ITEM_EN_SUELO,
            MensajeItemEnSuelo{idItem, x, y},
    };
}

MensajeServidor ProtocoloCliente::recibirItemDesaparecioSuelo() {
    uint16_t x = recibirDosBytes();
    uint16_t y = recibirDosBytes();

    return MensajeServidor{
            Opcode::ITEM_DESAPARECIO_SUELO,
            MensajeItemDesaparecioSuelo{x, y},
    };
}

//---------------

MensajeServidor ProtocoloCliente::recibirActualizarInventario() {
    uint8_t cantidad = recibirUnByte();

    std::vector<uint16_t> slots;
    slots.reserve(cantidad);

    for (uint8_t i = 0; i < cantidad; ++i) {
        slots.push_back(recibirDosBytes());
    }

    return MensajeServidor{
            Opcode::ACTUALIZAR_INVENTARIO,
            MensajeActualizarInventario{slots},
    };
}

MensajeServidor ProtocoloCliente::recibirListaItems() {
    uint8_t cantidad = recibirUnByte();

    std::vector<uint16_t> ids;
    ids.reserve(cantidad);

    for (uint8_t i = 0; i < cantidad; ++i) {
        ids.push_back(recibirDosBytes());
    }

    return MensajeServidor{
            Opcode::LISTA_ITEMS,
            MensajeListaItems{ids},
    };
}

//-------------

MensajeServidor ProtocoloCliente::recibirActualizarEquipamiento() {
    uint16_t arma = recibirDosBytes();
    uint16_t baculo = recibirDosBytes();
    uint16_t defensa = recibirDosBytes();
    uint16_t casco = recibirDosBytes();
    uint16_t escudo = recibirDosBytes();

    return MensajeServidor{
            Opcode::ACTUALIZAR_EQUIPAMIENTO,
            MensajeActualizarEquipamiento{
                    arma,
                    baculo,
                    defensa,
                    casco,
                    escudo,
            },
    };
}

MensajeServidor ProtocoloCliente::recibirMensajeChat() {
    std::string nickOrigen = recibirCadenaConMaximo(MAX_NICK);
    std::string mensaje = recibirCadenaConMaximo(MAX_CHAT);

    return MensajeServidor{
            Opcode::MENSAJE_CHAT,
            MensajeChat{nickOrigen, mensaje},
    };
}

MensajeServidor ProtocoloCliente::recibirMensajeClan() {
    uint8_t tipo = recibirUnByte();
    validarTipoClan(tipo);

    std::string nick = recibirCadenaConMaximo(MAX_NICK);

    return MensajeServidor{
            Opcode::MENSAJE_CLAN,
            MensajeClan{tipo, nick},
    };
}

MensajeServidor ProtocoloCliente::recibirResucitado() {
    uint16_t x = recibirDosBytes();
    uint16_t y = recibirDosBytes();

    return MensajeServidor{
            Opcode::RESUCITADO,
            MensajeResucitado{x, y},
    };
}

//---------------------

void ProtocoloCliente::validarTipoEntidad(uint8_t tipo) const {
    if (tipo > MAX_TIPO_ENTIDAD) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::TIPO_ENTIDAD_INVALIDO));
    }
}

void ProtocoloCliente::validarEstadoEntidad(uint8_t estado) const {
    if (estado > MAX_ESTADO_ENTIDAD) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::ESTADO_ENTIDAD_INVALIDO));
    }
}

void ProtocoloCliente::validarEsquivador(uint8_t esquivador) const {
    if (esquivador > MAX_ESQUIVADOR) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::CAMPO_INVALIDO));
    }
}

void ProtocoloCliente::validarTipoClan(uint8_t tipo) const {
    if (tipo > MAX_TIPO_CLAN) {
        throw std::runtime_error(
                MensajesErrorProtocolo::mensaje(
                        CodigoErrorProtocolo::CAMPO_INVALIDO));
    }
}