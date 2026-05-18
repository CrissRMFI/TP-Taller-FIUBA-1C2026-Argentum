#include "protocolo_cliente.h"

#include <stdexcept>
#include <utility>
#include <variant>

ProtocoloCliente::ProtocoloCliente(Socket&& skt) : Protocolo(std::move(skt)) {}

void ProtocoloCliente::enviarComando(const ComandoJugador& comando) {
    // envio del codigo de operacion
    enviarUnByte(static_cast<uint8_t>(comando.opcode));
    /*
     *  usar un switch case? que otra forma puede usarse?
     */
    switch (comando.opcode){
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
        throw std::runtime_error("opcode de cliente invalido");
    }
}

MensajeServidor ProtocoloCliente::recibirMensaje(){
    uint8_t opcode_recibido = recibirUnByte();
    Opcode opcode = static_cast<Opcode>(opcode_recibido);

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
    default:
        throw std::runtime_error("opcode de servidor invalido");
    }
}

void ProtocoloCliente::cerrarConexion(){
    if (!cerrado) {
        this->skt.shutdown(2); // Shutdown de lectura y escritura
        this->skt.close();
        this->cerrado = true;
    }
}

void ProtocoloCliente::enviarComandoMover(const ComandoMover& comando){
    validarDireccion(comando.direccion);
    enviarUnByte(comando.direccion);
}

void ProtocoloCliente::enviarComandoAtacar(const ComandoAtacar& comando){
    enviarDosBytes(comando.idObjetivo);
}

void ProtocoloCliente::enviarComandoMeditar(const ComandoMeditar& comando){
    (void)comando;
}

void ProtocoloCliente::enviarComandoResucitar(const ComandoResucitar& comando) {
    (void)comando;
}

void ProtocoloCliente::enviarComandoCurar(const ComandoCurar& comando) {
    enviarDosBytes(comando.idSacerdote);
}

void ProtocoloCliente::enviarComandoTomar(const ComandoTomar& comando) {
    (void)comando;
}

void ProtocoloCliente::enviarComandoTirar(const ComandoTirar& comando) {
    enviarUnByte(comando.indiceItem);
}

void ProtocoloCliente::enviarComandoEquipar(const ComandoEquipar& comando) {
    enviarUnByte(comando.indiceItem);
}

void ProtocoloCliente::enviarComandoComprar(const ComandoComprar& comando) {
    enviarDosBytes(comando.idItem);
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoVender(const ComandoVender& comando) {
    enviarUnByte(comando.indiceItem);
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoDepositarItem(const ComandoDepositarItem& comando) {
    enviarUnByte(comando.indiceItem);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoDepositarOro(const ComandoDepositarOro& comando) {
    enviarCuatroBytes(comando.monto);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoRetirarItem(const ComandoRetirarItem& comando) {
    enviarDosBytes(comando.idItem);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoRetirarOro(const ComandoRetirarOro& comando) {
    enviarCuatroBytes(comando.monto);
    enviarDosBytes(comando.idBanquero);
}

void ProtocoloCliente::enviarComandoListar(const ComandoListar& comando) {
    enviarDosBytes(comando.idNPC);
}

void ProtocoloCliente::enviarComandoChatGlobal(const ComandoChatGlobal& comando) {
    enviarCadenaConMaximo(comando.mensaje, MAX_CHAT);
}

void ProtocoloCliente::enviarComandoChatPrivado(const ComandoChatPrivado& comando) {
    enviarCadenaConMaximo(comando.nickDestino, MAX_NICK);
    enviarCadenaConMaximo(comando.mensaje, MAX_CHAT);
}

void ProtocoloCliente::enviarComandoFundarClan(const ComandoFundarClan& comando) {
    enviarCadenaConMaximo(comando.nombreClan, MAX_CLAN);
}

void ProtocoloCliente::enviarComandoUnirseClan(const ComandoUnirseClan& comando) {
    enviarCadenaConMaximo(comando.nombreClan, MAX_CLAN);
}

void ProtocoloCliente::enviarComandoRevisarClan(const ComandoRevisarClan& comando) {
    (void)comando;
}

void ProtocoloCliente::enviarComandoClanAceptar(const ComandoGestionMiembreClan& comando) {
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanRechazar(const ComandoGestionMiembreClan& comando) {
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanBan(const ComandoGestionMiembreClan& comando) {
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoClanKick(const ComandoGestionMiembreClan& comando) {
    enviarCadenaConMaximo(comando.nick, MAX_NICK);
}

void ProtocoloCliente::enviarComandoDejarClan(const ComandoDejarClan& comando) {
    (void)comando;
}

void ProtocoloCliente::validarDireccion(uint8_t direccion) const {
    if (direccion > MAX_DIRECCION) {
        throw std::runtime_error("direccion invalida");
    }
}

void ProtocoloCliente::validarCantidad(uint16_t cantidad) const {
    if (cantidad > MAX_CANTIDAD_UINT8) {
        throw std::runtime_error("cantidad invalida");
    }
}

void ProtocoloCliente::validarTipoEntidad(uint8_t tipo) const {
    if (tipo > MAX_TIPO_ENTIDAD) {
        throw std::runtime_error("tipo de entidad invalido");
    }
}

void ProtocoloCliente::validarEstadoEntidad(uint8_t estado) const {
    if (estado > MAX_ESTADO_ENTIDAD) {
        throw std::runtime_error("estado de entidad invalido");
    }
}

void ProtocoloCliente::validarEsquivador(uint8_t esquivador) const {
    if (esquivador > MAX_ESQUIVADOR) {
        throw std::runtime_error("esquivador invalido");
    }
}

void ProtocoloCliente::validarTipoClan(uint8_t tipo) const {
    if (tipo > MAX_TIPO_CLAN) {
        throw std::runtime_error("tipo de clan invalido");
    }
}

MensajeServidor ProtocoloCliente::recibirEstadoPersonaje() {
    return MensajeServidor{
            Opcode::ESTADO_PERSONAJE,
            MensajeEstadoPersonaje{
                    recibirDosBytes(),
                    recibirDosBytes(),
                    recibirDosBytes(),
                    recibirDosBytes(),
                    recibirCuatroBytes(),
                    recibirUnByte(),
                    recibirCuatroBytes(),
            },
    };
}

MensajeServidor ProtocoloCliente::recibirPosicionEntidad() {
    const uint16_t id = recibirDosBytes();
    const uint16_t x = recibirDosBytes();
    const uint16_t y = recibirDosBytes();
    const uint8_t tipo = recibirUnByte();
    const uint8_t estado = recibirUnByte();

    validarTipoEntidad(tipo);
    validarEstadoEntidad(estado);

    return MensajeServidor{
            Opcode::POSICION_ENTIDAD,
            MensajePosicionEntidad{id, x, y, tipo, estado},
    };
}

MensajeServidor ProtocoloCliente::recibirEntidadDesaparecio() {
    return MensajeServidor{
            Opcode::ENTIDAD_DESAPARECIO,
            MensajeEntidadDesaparecio{recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirDanoRecibido() {
    return MensajeServidor{
            Opcode::DANIO_RECIBIDO,
            MensajeDanoRecibido{recibirDosBytes(), recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirDanoProducido() {
    return MensajeServidor{
            Opcode::DANIO_PRODUCIDO,
            MensajeDanoProducido{recibirDosBytes(), recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirEsquive() {
    const uint16_t idEntidad = recibirDosBytes();
    const uint8_t esquivador = recibirUnByte();
    validarEsquivador(esquivador);

    return MensajeServidor{
            Opcode::ESQUIVE,
            MensajeEsquive{idEntidad, esquivador},
    };
}

MensajeServidor ProtocoloCliente::recibirMuerteEntidad() {
    return MensajeServidor{
            Opcode::MUERTE_ENTIDAD,
            MensajeMuerteEntidad{recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirItemEnSuelo() {
    return MensajeServidor{
            Opcode::ITEM_EN_SUELO,
            MensajeItemEnSuelo{recibirDosBytes(), recibirDosBytes(), recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirItemDesaparecioSuelo() {
    return MensajeServidor{
            Opcode::ITEM_DESAPARECIO_SUELO,
            MensajeItemDesaparecioSuelo{recibirDosBytes(), recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirActualizarInventario() {
    uint16_t cantidad = recibirUnByte();
    validarCantidad(cantidad);

    std::vector<uint16_t> slots;
    slots.reserve(cantidad);
    for (uint16_t i = 0; i < cantidad; ++i) {
        slots.push_back(recibirDosBytes());
    }

    return MensajeServidor{
            Opcode::ACTUALIZAR_INVENTARIO,
            MensajeActualizarInventario{slots},
    };
}

MensajeServidor ProtocoloCliente::recibirActualizarEquipamiento() {
    return MensajeServidor{
            Opcode::ACTUALIZAR_EQUIPAMIENTO,
            MensajeActualizarEquipamiento{
                    recibirDosBytes(),
                    recibirDosBytes(),
                    recibirDosBytes(),
                    recibirDosBytes(),
                    recibirDosBytes(),
            },
    };
}

MensajeServidor ProtocoloCliente::recibirMensajeChat() {
    return MensajeServidor{
            Opcode::MENSAJE_CHAT,
            MensajeChat{
                    recibirCadenaConMaximo(MAX_NICK),
                    recibirCadenaConMaximo(MAX_CHAT),
            },
    };
}

MensajeServidor ProtocoloCliente::recibirMensajeClan() {
    const uint8_t tipo = recibirUnByte();
    validarTipoClan(tipo);

    return MensajeServidor{
            Opcode::MENSAJE_CLAN,
            MensajeClan{tipo, recibirCadenaConMaximo(MAX_NICK)},
    };
}

MensajeServidor ProtocoloCliente::recibirResucitado() {
    return MensajeServidor{
            Opcode::RESUCITADO,
            MensajeResucitado{recibirDosBytes(), recibirDosBytes()},
    };
}

MensajeServidor ProtocoloCliente::recibirListaItems() {
    uint16_t cantidad = recibirUnByte();
    validarCantidad(cantidad);

    std::vector<uint16_t> ids;
    ids.reserve(cantidad);
    for (uint16_t i = 0; i < cantidad; ++i) {
        ids.push_back(recibirDosBytes());
    }

    return MensajeServidor{
            Opcode::LISTA_ITEMS,
            MensajeListaItems{ids},
    };
}

MensajeServidor ProtocoloCliente::recibirErrorAccion() {
    throw std::runtime_error("error de accion recibido");
}
