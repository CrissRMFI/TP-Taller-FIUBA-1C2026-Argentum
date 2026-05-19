#include "juego.h"

#include <list>
#include <utility>
#include <variant>

#include "objeto/catalogo_items.h"


Juego::Juego(const ConfigJuego& cfg, CatalogoItems cat)
    : cfg(cfg), catalogo(std::move(cat)), proximoIdClan(1) {}

void Juego::conectarJugador(uint16_t id, const std::string& nombre,
                             ClasePersonaje clase, Raza raza, Posicion posicion) {
    auto it = jugadoresDesconectados.find(id);
    if (it != jugadoresDesconectados.end()) {
        jugadoresConectados.emplace(id, std::move(it->second));
        jugadoresDesconectados.erase(it);
    } else {
        jugadoresConectados.emplace(id, Jugador(id, nombre, clase, raza, posicion, cfg));
    }
}

void Juego::desconectarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    if (it == jugadoresConectados.end()) return;
    jugadoresDesconectados.emplace(id, std::move(it->second));
    jugadoresConectados.erase(it);
}

Jugador* Juego::buscarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    return (it != jugadoresConectados.end()) ? &it->second : nullptr;
}

Jugador* Juego::buscarJugadorPorNick(const std::string& nick) {
    for (auto& [id, j] : jugadoresConectados)
        if (j.getNombre() == nick) return &j;
    return nullptr;
}

Clan* Juego::buscarClanPorNombre(const std::string& nombre) {
    for (auto& [id, c] : clanes)
        if (c.getNombre() == nombre) return &c;
    return nullptr;
}

std::string Juego::nickDe(uint16_t idJugador) {
    if (auto it = jugadoresConectados.find(idJugador); it != jugadoresConectados.end())
        return it->second.getNombre();
    if (auto it = jugadoresDesconectados.find(idJugador); it != jugadoresDesconectados.end())
        return it->second.getNombre();
    return "";
}

MensajeSalida Juego::armarError(uint16_t idCliente, CodigoErrorAccion cod) {
    return { TipoDestino::UNO, idCliente,
             { Opcode::ERROR_ACCION, MensajeErrorAccion{ cod } } };
}

MensajeSalida Juego::armarEstado(uint16_t idCliente, const Jugador& jugador) {
    return { TipoDestino::UNO, idCliente,
             { Opcode::ESTADO_PERSONAJE, MensajeEstadoPersonaje{
                 jugador.getVidaActual(), jugador.getVidaMax(),
                 jugador.getManaActual(), jugador.getManaMax(),
                 jugador.getOro(), jugador.getNivel(), jugador.getExperiencia()
             }}};
}


std::list<MensajeSalida> Juego::ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando) {
    if (comando.opcode != Opcode::MEDITAR) {
        Jugador* jugador = buscarJugador(idCliente);
        if (jugador) jugador->cancelarMeditacion();
    }

    try {
        switch (comando.opcode) {
          case Opcode::MEDITAR:
            return ejecutarMeditar(idCliente);
          case Opcode::RESUCITAR:
            return ejecutarResucitar(idCliente);
          case Opcode::TOMAR:
            return ejecutarTomar(idCliente);
          case Opcode::REVISAR_CLAN:
            return ejecutarRevisarClan(idCliente);
          case Opcode::DEJAR_CLAN:
            return ejecutarDejarClan(idCliente);
          case Opcode::MOVER:
            return ejecutarMover(idCliente, std::get<ComandoMover>(comando.payload));
          case Opcode::ATACAR:
            return ejecutarAtacar(idCliente, std::get<ComandoAtacar>(comando.payload));
          case Opcode::TIRAR:
            return ejecutarTirar(idCliente, std::get<ComandoTirar>(comando.payload));
          case Opcode::EQUIPAR:
            return ejecutarEquipar(idCliente, std::get<ComandoEquipar>(comando.payload));
          case Opcode::COMPRAR:
            return ejecutarComprar(idCliente, std::get<ComandoComprar>(comando.payload));
          case Opcode::VENDER:
            return ejecutarVender(idCliente, std::get<ComandoVender>(comando.payload));
          case Opcode::DEPOSITAR_ITEM:
            return ejecutarDepositarItem(idCliente, std::get<ComandoDepositarItem>(comando.payload));
          case Opcode::DEPOSITAR_ORO:
            return ejecutarDepositarOro(idCliente, std::get<ComandoDepositarOro>(comando.payload));
          case Opcode::RETIRAR_ITEM:
            return ejecutarRetirarItem(idCliente, std::get<ComandoRetirarItem>(comando.payload));
          case Opcode::RETIRAR_ORO:
            return ejecutarRetirarOro(idCliente, std::get<ComandoRetirarOro>(comando.payload));
          case Opcode::LISTAR:
            return ejecutarListar(idCliente, std::get<ComandoListar>(comando.payload));
          case Opcode::CURAR:
            return ejecutarCurar(idCliente, std::get<ComandoCurar>(comando.payload));
          case Opcode::CHAT_GLOBAL:
            return ejecutarChatGlobal(idCliente, std::get<ComandoChatGlobal>(comando.payload));
          case Opcode::CHAT_PRIVADO:
            return ejecutarChatPrivado(idCliente, std::get<ComandoChatPrivado>(comando.payload));
          case Opcode::FUNDAR_CLAN:
            return ejecutarFundarClan(idCliente, std::get<ComandoFundarClan>(comando.payload));
          case Opcode::UNIRSE_CLAN:
            return ejecutarUnirseClan(idCliente, std::get<ComandoUnirseClan>(comando.payload));
          case Opcode::CLAN_ACEPTAR:
          case Opcode::CLAN_RECHAZAR:
          case Opcode::CLAN_BAN:
          case Opcode::CLAN_KICK:
            return ejecutarGestionMiembroClan(idCliente, std::get<ComandoGestionMiembreClan>(comando.payload), comando.opcode);
          default:
            return {};
        }
    } catch (const std::bad_variant_access&) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }
}

// ─── Tick del mundo ───────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::actualizar() {
    std::list<MensajeSalida> mensajes;
    for (auto& [id, jugador] : jugadoresConectados) {
        jugador.recuperar(cfg.tickMs / 1000.0f);
        mensajes.push_back(armarEstado(id, jugador));
    }
    // TODO: mover criaturas, aplicar aggro, respawn, expirar ítems del suelo
    return mensajes;
}

std::list<uint16_t> Juego::criaturasCerca(Posicion posicion) {
    // TODO: filtrar criaturasEnMapa por distancia al jugador
    return {};
}

// ─── Meditar ─────────────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::ejecutarMeditar(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (!jugador->estaVivo() || !jugador->puedeMediatar())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    jugador->meditar();
    return { armarEstado(idCliente, *jugador) };
}

// ─── Chat ─────────────────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::ejecutarChatGlobal(uint16_t idCliente, const ComandoChatGlobal& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    return {{ TipoDestino::TODOS, 0,
              { Opcode::MENSAJE_CHAT, MensajeChat{ jugador->getNombre(), comando.mensaje } } }};
}

std::list<MensajeSalida> Juego::ejecutarChatPrivado(uint16_t idCliente, const ComandoChatPrivado& comando) {
    Jugador* emisor = buscarJugador(idCliente);
    if (!emisor) return {};

    Jugador* destino = buscarJugadorPorNick(comando.nickDestino);
    if (!destino)
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };

    uint16_t idDestino = 0;
    for (auto& [id, jugador] : jugadoresConectados)
        if (&jugador == destino) { idDestino = id; break; }

    return {{ TipoDestino::UNO, idDestino,
              { Opcode::MENSAJE_CHAT, MensajeChat{ emisor->getNombre(), comando.mensaje } } }};
}

// ─── Clan ─────────────────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::ejecutarFundarClan(uint16_t idCliente, const ComandoFundarClan& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (jugador->tieneClan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    if (jugador->getNivel() < cfg.clanNivelMinimo)
        return { armarError(idCliente, CodigoErrorAccion::NIVEL_INSUFICIENTE) };
    if (buscarClanPorNombre(comando.nombreClan))
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    uint16_t idClan = proximoIdClan++;
    clanes.emplace(idClan, Clan(idClan, comando.nombreClan, idCliente));
    jugador->asignarClan(idClan);
    jugador->marcarFundadorClan();

    return {{ TipoDestino::UNO, idCliente,
              { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Fundado, comando.nombreClan } } }};
}

std::list<MensajeSalida> Juego::ejecutarUnirseClan(uint16_t idCliente, const ComandoUnirseClan& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (jugador->tieneClan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    Clan* clan = buscarClanPorNombre(comando.nombreClan);
    if (!clan)
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    if (clan->estaBaneado(idCliente))
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    if ((int)(clan->cantidadMiembros()) >= cfg.clanMaxMiembros)
        return { armarError(idCliente, CodigoErrorAccion::CLAN_LLENO) };

    clan->pedirUnirse(idCliente);

    // Notificar al fundador si está conectado
    std::list<MensajeSalida> mensajes;
    uint16_t idClan = clan->getId();
    const std::string nickSolicitante = nickDe(idCliente);
    for (auto& [id, jugador] : jugadoresConectados) {
        if (jugador.fundo_clan() && jugador.getClan() == idClan) {
            mensajes.push_back({ TipoDestino::UNO, id,
                { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::MiembroPendiente, nickSolicitante } } });
        }
    }
    return mensajes;
}

std::list<MensajeSalida> Juego::ejecutarDejarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->tieneClan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    if (jugador->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    clanes.at(jugador->getClan()).eliminarMiembro(idCliente);
    jugador->salirClan();
    return {};
}

std::list<MensajeSalida> Juego::ejecutarRevisarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->tieneClan() || !jugador->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    Clan& clan = clanes.at(jugador->getClan());
    std::list<MensajeSalida> mensajes;

    for (uint16_t idPend : clan.obtenerPendientes())
        mensajes.push_back({ TipoDestino::UNO, idCliente,
            { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::MiembroPendiente, nickDe(idPend) } } });

    for (uint16_t idMiembro : clan.obtenerMiembros())
        mensajes.push_back({ TipoDestino::UNO, idCliente,
            { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::MiembroActivo, nickDe(idMiembro) } } });

    return mensajes;
}

std::list<MensajeSalida> Juego::ejecutarGestionMiembroClan(uint16_t idCliente, const ComandoGestionMiembreClan& comando, Opcode accion) {
    Jugador* lider = buscarJugador(idCliente);
    if (!lider || !lider->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    Clan& clan = clanes.at(lider->getClan());
    Jugador* objetivo = buscarJugadorPorNick(comando.nick);

    // Para operaciones sobre miembros activos/pendientes el objetivo puede estar desconectado.
    // Buscamos su id iterando ambos mapas.
    uint16_t idObjetivo = 0;
    for (auto& [id, j] : jugadoresConectados)
        if (j.getNombre() == comando.nick) { idObjetivo = id; break; }
    if (!idObjetivo)
        for (auto& [id, j] : jugadoresDesconectados)
            if (j.getNombre() == comando.nick) { idObjetivo = id; break; }

    if (!idObjetivo)
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };

    if (idObjetivo == idCliente)
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    if ((accion == Opcode::CLAN_KICK || accion == Opcode::CLAN_BAN) && clan.esFundador(idObjetivo))
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    std::list<MensajeSalida> msgs;

    switch (accion) {
      case Opcode::CLAN_ACEPTAR: {
        if (!clan.estaPendiente(idObjetivo))
            return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        clan.agregarMiembro(idObjetivo);
        // Actualizar el jugador conectado o desconectado
        if (objetivo) {
            objetivo->asignarClan(lider->getClan());
            msgs.push_back({ TipoDestino::UNO, idObjetivo,
                { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Aceptado, clan.getNombre() } } });
        } else {
            auto itDesc = jugadoresDesconectados.find(idObjetivo);
            if (itDesc != jugadoresDesconectados.end())
                itDesc->second.asignarClan(lider->getClan());
        }
        break;
      }

      case Opcode::CLAN_RECHAZAR:
        if (!clan.estaPendiente(idObjetivo))
            return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        clan.eliminarMiembro(idObjetivo);
        if (objetivo)
            msgs.push_back({ TipoDestino::UNO, idObjetivo,
                { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Rechazado, clan.getNombre() } } });
        break;

      case Opcode::CLAN_BAN:
        if (!clan.esMiembro(idObjetivo) && !clan.estaPendiente(idObjetivo))
            return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        clan.banearMiembro(idObjetivo);
        if (objetivo) {
            objetivo->salirClan();
            msgs.push_back({ TipoDestino::UNO, idObjetivo,
                { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Baneado, clan.getNombre() } } });
        } else {
            auto itDesc = jugadoresDesconectados.find(idObjetivo);
            if (itDesc != jugadoresDesconectados.end())
                itDesc->second.salirClan();
        }
        break;

      case Opcode::CLAN_KICK:
        if (!clan.esMiembro(idObjetivo))
            return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        clan.eliminarMiembro(idObjetivo);
        if (objetivo) {
            objetivo->salirClan();
            msgs.push_back({ TipoDestino::UNO, idObjetivo,
                { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Kickeado, clan.getNombre() } } });
        } else {
            auto itDesc = jugadoresDesconectados.find(idObjetivo);
            if (itDesc != jugadoresDesconectados.end())
                itDesc->second.salirClan();
        }
        break;

      default:
        break;
    }
    return msgs;
}

// ─── Mapa/Mundo ──────────────────────────────────────────

std::list<MensajeSalida> Juego::ejecutarResucitar(uint16_t /*idCliente*/) {
    // TODO: verificar que es fantasma, buscar sacerdote cercano o ciudad más cercana
    return {};
}

std::list<MensajeSalida> Juego::ejecutarTomar(uint16_t /*idCliente*/) {
    // TODO: verificar vivo, ítem en posición, espacio en inventario; mover del mapa al inventario
    return {};
}

std::list<MensajeSalida> Juego::ejecutarMover(uint16_t /*idCliente*/, const ComandoMover& /*cmd*/) {
    // TODO: validar límites, celda transitable y libre; actualizar posición en el mapa
    return {};
}

std::list<MensajeSalida> Juego::ejecutarAtacar(uint16_t /*idCliente*/, const ComandoAtacar& /*cmd*/) {
    // TODO: verificar vivo, rango, fair-play, zona segura; aplicar daño; manejar muerte
    return {};
}

std::list<MensajeSalida> Juego::ejecutarTirar(uint16_t /*idCliente*/, const ComandoTirar& /*cmd*/) {
    // TODO: verificar vivo, ítem en inventario, celda libre; mover del inventario al mapa
    return {};
}

std::list<MensajeSalida> Juego::ejecutarEquipar(uint16_t /*idCliente*/, const ComandoEquipar& /*cmd*/) {
    // TODO: verificar vivo, ítem en inventario, restricciones de raza/clase
    return {};
}

std::list<MensajeSalida> Juego::ejecutarComprar(uint16_t /*idCliente*/, const ComandoComprar& /*cmd*/) {
    // TODO: verificar vivo, comerciante cercano, ítem disponible, oro suficiente, espacio
    return {};
}

std::list<MensajeSalida> Juego::ejecutarVender(uint16_t /*idCliente*/, const ComandoVender& /*cmd*/) {
    // TODO: verificar vivo, comerciante cercano, ítem en inventario
    return {};
}

std::list<MensajeSalida> Juego::ejecutarDepositarItem(uint16_t /*idCliente*/, const ComandoDepositarItem& /*cmd*/) {
    // TODO: verificar vivo, banquero cercano, ítem en inventario, espacio en banco
    return {};
}

std::list<MensajeSalida> Juego::ejecutarDepositarOro(uint16_t /*idCliente*/, const ComandoDepositarOro& /*cmd*/) {
    // TODO: verificar vivo, banquero cercano, oro suficiente
    return {};
}

std::list<MensajeSalida> Juego::ejecutarRetirarItem(uint16_t /*idCliente*/, const ComandoRetirarItem& /*cmd*/) {
    // TODO: verificar vivo, banquero cercano, ítem en banco, espacio en inventario
    return {};
}

std::list<MensajeSalida> Juego::ejecutarRetirarOro(uint16_t /*idCliente*/, const ComandoRetirarOro& /*cmd*/) {
    // TODO: verificar vivo, banquero cercano, oro en banco
    return {};
}

std::list<MensajeSalida> Juego::ejecutarListar(uint16_t /*idCliente*/, const ComandoListar& /*cmd*/) {
    // TODO: verificar vivo, NPC cercano; devolver lista de ítems/precios
    return {};
}

std::list<MensajeSalida> Juego::ejecutarCurar(uint16_t /*idCliente*/, const ComandoCurar& /*cmd*/) {
    // TODO: verificar vivo, sacerdote cercano; aplicar curación
    return {};
}
