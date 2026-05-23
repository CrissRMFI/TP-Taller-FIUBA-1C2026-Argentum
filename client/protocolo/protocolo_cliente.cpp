#include "protocolo_cliente.h"

#include <stdexcept>
#include <utility>
#include <variant>

#include "../../common/mensajes/codigo_error_accion.h"
#include "../../common/mensajes/codigo_error_protocolo.h"
#include "../../common/mensajes/mensajes_error_accion.h"
#include "../../common/mensajes/mensajes_error_protocolo.h"

ProtocoloCliente::ProtocoloCliente(Socket&& skt) : Protocolo(std::move(skt)) {}

void ProtocoloCliente::cerrarConexion() {
    if (!cerrado) {
        skt.close();
        cerrado = true;
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
            throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::OPCODE_CLIENTE_INVALIDO));
    }
}

MensajeServidor ProtocoloCliente::recibirMensaje() {
    uint8_t opcode_recibido = recibirUnByte();

    Opcode opcode = (Opcode)(opcode_recibido);

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
            throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::OPCODE_SERVIDOR_INVALIDO));
    }
}

void ProtocoloCliente::validarDireccion(uint8_t direccion) const {
    if (direccion > MAX_DIRECCION) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::DIRECCION_INVALIDA));
    }
}

void ProtocoloCliente::validarTipoEntidad(uint8_t tipo) const {
    if (tipo > MAX_TIPO_ENTIDAD) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::TIPO_ENTIDAD_INVALIDO));
    }
}

void ProtocoloCliente::validarEstadoEntidad(uint8_t estado) const {
    if (estado > MAX_ESTADO_ENTIDAD) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::ESTADO_ENTIDAD_INVALIDO));
    }
}

void ProtocoloCliente::validarEsquivador(uint8_t esquivador) const {
    if (esquivador > MAX_ESQUIVADOR) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CAMPO_INVALIDO));
    }
}

void ProtocoloCliente::validarCantidad(uint16_t cantidad) const {
    if (cantidad > MAX_CANTIDAD_UINT8) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CAMPO_INVALIDO));
    }
}

void ProtocoloCliente::validarTipoClan(uint8_t tipo) const {
    if (tipo > MAX_TIPO_CLAN) {
        throw std::runtime_error(MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo::CAMPO_INVALIDO));
    }
}

void ProtocoloCliente::enviarComandoMover(const ComandoMover& comando) {
    validarDireccion(comando.direccion);

    enviarUnByte((uint8_t)(Opcode::MOVER));
    enviarUnByte(comando.direccion);
}

void ProtocoloCliente::enviarComandoAtacar(const ComandoAtacar& comando) {
    enviarUnByte((uint8_t)(Opcode::ATACAR));
    enviarDosBytes(comando.idObjetivo);
}

void ProtocoloCliente::enviarComandoMeditar(const ComandoMeditar&) {
    enviarUnByte((uint8_t)(Opcode::MEDITAR));
}

void ProtocoloCliente::enviarComandoResucitar(const ComandoResucitar&) {
    enviarUnByte((uint8_t)(Opcode::RESUCITAR));
}

void ProtocoloCliente::enviarComandoCurar(const ComandoCurar& comando) {
    enviarUnByte((uint8_t)(Opcode::CURAR));
    enviarDosBytes(comando.idSacerdote);
}

void ProtocoloCliente::enviarComandoTomar(const ComandoTomar&) {
    enviarUnByte((uint8_t)(Opcode::TOMAR));
}

void ProtocoloCliente::enviarComandoTirar(const ComandoTirar& comando) {
    enviarUnByte((uint8_t)(Opcode::TIRAR));
    enviarUnByte(comando.indiceItem);
}

void ProtocoloCliente::enviarComandoEquipar(const ComandoEquipar& comando) {
    enviarUnByte((uint8_t)(Opcode::EQUIPAR));
    enviarUnByte(comando.indiceItem);
}

void ProtocoloCliente::enviarComandoComprar(const ComandoComprar& comando) {
    enviarUnByte((uint8_t)(Opcode::COMPRAR));
    enviarDosBytes(comando.idItem);
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoVender(const ComandoVender& comando) {
    enviarUnByte((uint8_t)(Opcode::VENDER));
    enviarUnByte(comando.indiceItem);
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoDepositarItem(const ComandoDepositarItem& comando) {
    enviarUnByte((uint8_t)(Opcode::DEPOSITAR_ITEM));
    enviarUnByte(comando.indiceItem);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoDepositarOro(const ComandoDepositarOro& comando) {
    enviarUnByte((uint8_t)(Opcode::DEPOSITAR_ORO));
    enviarCuatroBytes(comando.monto);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoRetirarItem(const ComandoRetirarItem& comando) {
    enviarUnByte((uint8_t)(Opcode::RETIRAR_ITEM));
    enviarDosBytes(comando.idItem);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoRetirarOro(const ComandoRetirarOro& comando) {
    enviarUnByte((uint8_t)(Opcode::RETIRAR_ORO));
    enviarCuatroBytes(comando.monto);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoListar(const ComandoListar& comando) {
    enviarUnByte((uint8_t)(Opcode::LISTAR));
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoChatGlobal(const ComandoChatGlobal& comando) {
    enviarUnByte((uint8_t)(Opcode::CHAT_GLOBAL));
    enviarCadenaConMaximo(comando.mensaje, MAX_CHAT);
}

void ProtocoloCliente::enviarComandoChatPrivado(const ComandoChatPrivado& comando) {
    enviarUnByte((uint8_t)(Opcode::CHAT_PRIVADO));
    enviarCadenaConMaximo(comando.nickDestino, MAX_NICK);
    enviarCadenaConMaximo(comando.mensaje, MAX_CHAT);
}

void ProtocoloCliente::enviarComandoFundarClan(const ComandoFundarClan& comando) {
    enviarUnByte((uint8_t)(Opcode::FUNDAR_CLAN));
    enviarCadenaConMaximo(comando.nombreClan, MAX_CLAN);
}

void ProtocoloCliente::enviarComandoUnirseClan(const ComandoUnirseClan& comando) {
    enviarUnByte((uint8_t)(Opcode::UNIRSE_CLAN));
    enviarCadenaConMaximo(comando.nombreClan, MAX_CLAN);
}

void ProtocoloCliente::enviarComandoRevisarClan(const ComandoRevisarClan&) {
    enviarUnByte((uint8_t)(Opcode::REVISAR_CLAN));
}

void ProtocoloCliente::enviarComandoClanAceptar(const ComandoGestionMiembreClan& comando) {
    enviarUnByte((uint8_t)(Opcode::CLAN_ACEPTAR));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanRechazar(const ComandoGestionMiembreClan& comando) {
    enviarUnByte((uint8_t)(Opcode::CLAN_RECHAZAR));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanBan(const ComandoGestionMiembreClan& comando) {
    enviarUnByte((uint8_t)(Opcode::CLAN_BAN));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanKick(const ComandoGestionMiembreClan& comando) {
    enviarUnByte((uint8_t)(Opcode::CLAN_KICK));
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoDejarClan(const ComandoDejarClan&) {
    enviarUnByte((uint8_t)(Opcode::DEJAR_CLAN));
}

MensajeServidor ProtocoloCliente::recibirEstadoPersonaje() {
    MensajeEstadoPersonaje mensaje;

    mensaje.vidaActual = recibirDosBytes();
    mensaje.vidaMax = recibirDosBytes();
    mensaje.manaActual = recibirDosBytes();
    mensaje.manaMax = recibirDosBytes();
    mensaje.oro = recibirCuatroBytes();
    mensaje.nivel = recibirUnByte();
    mensaje.experiencia = recibirCuatroBytes();

    return MensajeServidor{Opcode::ESTADO_PERSONAJE, mensaje};
}

MensajeServidor ProtocoloCliente::recibirPosicionEntidad() {
    MensajePosicionEntidad mensaje;

    mensaje.id = recibirDosBytes();
    mensaje.x = recibirDosBytes();
    mensaje.y = recibirDosBytes();
    mensaje.tipo = recibirUnByte();
    mensaje.estado = recibirUnByte();

    validarTipoEntidad(mensaje.tipo);
    validarEstadoEntidad(mensaje.estado);

    return MensajeServidor{Opcode::POSICION_ENTIDAD, mensaje};
}

MensajeServidor ProtocoloCliente::recibirEntidadDesaparecio() {
    MensajeEntidadDesaparecio mensaje;

    mensaje.id = recibirDosBytes();

    return MensajeServidor{Opcode::ENTIDAD_DESAPARECIO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirDanoRecibido() {
    MensajeDanoRecibido mensaje;

    mensaje.cantidad = recibirDosBytes();
    mensaje.idAtacante = recibirDosBytes();

    return MensajeServidor{Opcode::DANIO_RECIBIDO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirDanoProducido() {
    MensajeDanoProducido mensaje;

    mensaje.cantidad = recibirDosBytes();
    mensaje.idObjetivo = recibirDosBytes();

    return MensajeServidor{Opcode::DANIO_PRODUCIDO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirEsquive() {
    MensajeEsquive mensaje;

    mensaje.idEntidad = recibirDosBytes();
    mensaje.esquivador = recibirUnByte();

    validarEsquivador(mensaje.esquivador);

    return MensajeServidor{Opcode::ESQUIVE, mensaje};
}

MensajeServidor ProtocoloCliente::recibirMuerteEntidad() {
    MensajeMuerteEntidad mensaje;

    mensaje.id = recibirDosBytes();

    return MensajeServidor{Opcode::MUERTE_ENTIDAD, mensaje};
}

MensajeServidor ProtocoloCliente::recibirItemEnSuelo() {
    MensajeItemEnSuelo mensaje;

    mensaje.idItem = recibirDosBytes();
    mensaje.x = recibirDosBytes();
    mensaje.y = recibirDosBytes();

    return MensajeServidor{Opcode::ITEM_EN_SUELO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirItemDesaparecioSuelo() {
    MensajeItemDesaparecioSuelo mensaje;

    mensaje.x = recibirDosBytes();
    mensaje.y = recibirDosBytes();

    return MensajeServidor{Opcode::ITEM_DESAPARECIO_SUELO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirActualizarInventario() {
    MensajeActualizarInventario mensaje;

    uint8_t cantidad = recibirUnByte();
    mensaje.slots.reserve(cantidad);

    for (uint8_t i = 0; i < cantidad; ++i) {
        mensaje.slots.push_back(recibirDosBytes());
    }

    return MensajeServidor{Opcode::ACTUALIZAR_INVENTARIO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirActualizarEquipamiento() {
    MensajeActualizarEquipamiento mensaje;

    mensaje.arma = recibirDosBytes();
    mensaje.baculo = recibirDosBytes();
    mensaje.defensa = recibirDosBytes();
    mensaje.casco = recibirDosBytes();
    mensaje.escudo = recibirDosBytes();

    return MensajeServidor{Opcode::ACTUALIZAR_EQUIPAMIENTO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirMensajeChat() {
    MensajeChat mensaje;

    mensaje.nickOrigen = recibirCadenaConMaximo(MAX_NICK);
    mensaje.mensaje = recibirCadenaConMaximo(MAX_CHAT);

    return MensajeServidor{Opcode::MENSAJE_CHAT, mensaje};
}

MensajeServidor ProtocoloCliente::recibirMensajeClan() {
    MensajeClan mensaje;

    mensaje.tipo = recibirUnByte();
    mensaje.nick = recibirCadenaConMaximo(MAX_NICK);

    validarTipoClan(mensaje.tipo);

    return MensajeServidor{Opcode::MENSAJE_CLAN, mensaje};
}

MensajeServidor ProtocoloCliente::recibirResucitado() {
    MensajeResucitado mensaje;

    mensaje.x = recibirDosBytes();
    mensaje.y = recibirDosBytes();

    return MensajeServidor{Opcode::RESUCITADO, mensaje};
}

MensajeServidor ProtocoloCliente::recibirListaItems() {
    MensajeListaItems mensaje;

    uint8_t cantidad = recibirUnByte();
    mensaje.ids.reserve(cantidad);

    for (uint8_t i = 0; i < cantidad; ++i) {
        mensaje.ids.push_back(recibirDosBytes());
    }

    return MensajeServidor{Opcode::LISTA_ITEMS, mensaje};
}

MensajeServidor ProtocoloCliente::recibirErrorAccion() {
    uint8_t codigo = recibirUnByte();
    throw std::runtime_error(MensajesErrorAccion::mensaje((CodigoErrorAccion)(codigo)));
}
