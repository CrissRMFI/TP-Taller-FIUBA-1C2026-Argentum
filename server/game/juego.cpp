#include "juego.h"

#include <list>
#include <utility>
#include <variant>

#include "objeto/catalogo_items.h"

namespace {
constexpr uint8_t TIPO_ENTIDAD_PERSONAJE = 0;
constexpr uint8_t ESTADO_ENTIDAD_VIVO = 0;
constexpr uint8_t ESTADO_ENTIDAD_FANTASMA = 1;
constexpr uint8_t ESTADO_ENTIDAD_MEDITANDO = 2;

uint8_t estadoEntidadDe(const Jugador& jugador) {
    if (jugador.enMeditacion()) {
        return ESTADO_ENTIDAD_MEDITANDO;
    }

    if (jugador.esFantasma()) {
        return ESTADO_ENTIDAD_FANTASMA;
    }

    return ESTADO_ENTIDAD_VIVO;
}
}


Juego::Juego(const ConfigJuego& cfg, CatalogoItems&& cat)
    : cfg(cfg), catalogo(std::move(cat)), proximoIdClan(1) {}
    
std::list<MensajeSalida> Juego::conectarJugador(uint16_t id, const std::string& nombre,
                                                ClasePersonaje clase, Raza raza, Posicion posicion) {
    auto itDesconectado = jugadoresDesconectados.end();
    
    for (auto it = jugadoresDesconectados.begin(); it != jugadoresDesconectados.end(); ++it) {
      if (it->second.getNombre() == nombre) {
        itDesconectado = it;
        break;
      }
    }

    if (itDesconectado != jugadoresDesconectados.end()) {
      jugadoresConectados.emplace(id, std::move(itDesconectado->second));
      jugadoresConectados.at(id).actualizarId(id);
      jugadoresDesconectados.erase(itDesconectado);
    } else {
      jugadoresConectados.emplace(id, Jugador(id, nombre, clase, raza, posicion, cfg));
    }

}

std::list<MensajeSalida> Juego::desconectarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    if (it == jugadoresConectados.end()) return {};

    std::list<MensajeSalida> mensajes = {
        armarDesaparicion(id)
    };

    jugadoresDesconectados.emplace(id, std::move(it->second));
    jugadoresConectados.erase(it);
    return mensajes;
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

MensajeSalida Juego::armarInventario(uint16_t idCliente, const Jugador& jugador) {
    return { TipoDestino::UNO, idCliente,
             { Opcode::ACTUALIZAR_INVENTARIO,
               MensajeActualizarInventario{ jugador.getSlotsInventario() } } };
}

MensajeSalida Juego::armarEquipamiento(uint16_t idCliente, const Jugador& jugador) {
    return { TipoDestino::UNO, idCliente,
             { Opcode::ACTUALIZAR_EQUIPAMIENTO,
               MensajeActualizarEquipamiento{
                   jugador.getArmaEquipada(),
                   jugador.getBaculoEquipado(),
                   jugador.getDefensaEquipada(),
                   jugador.getCascoEquipado(),
                   jugador.getEscudoEquipado()
               } } };
}

MensajeSalida Juego::armarPosicion(const Jugador& jugador) {
    Posicion posicion = jugador.getPosicion();
    return { TipoDestino::TODOS, 0,
             { Opcode::POSICION_ENTIDAD,
               MensajePosicionEntidad{
                   jugador.getId(),
                   posicion.x,
                   posicion.y,
                   TIPO_ENTIDAD_PERSONAJE,
                   estadoEntidadDe(jugador)
               } } };
}

MensajeSalida Juego::armarPosicionPara(uint16_t idCliente, const Jugador& jugador) {
    Posicion posicion = jugador.getPosicion();
    return { TipoDestino::UNO, idCliente,
             { Opcode::POSICION_ENTIDAD,
               MensajePosicionEntidad{
                   jugador.getId(),
                   posicion.x,
                   posicion.y,
                   TIPO_ENTIDAD_PERSONAJE,
                   estadoEntidadDe(jugador)
               } } };
}

MensajeSalida Juego::armarPosicionExcepto(uint16_t idClienteExcluido, const Jugador& jugador) {
    Posicion posicion = jugador.getPosicion();
    return { TipoDestino::TODOS_EXCEPTO_UNO, idClienteExcluido,
             { Opcode::POSICION_ENTIDAD,
               MensajePosicionEntidad{
                   jugador.getId(),
                   posicion.x,
                   posicion.y,
                   TIPO_ENTIDAD_PERSONAJE,
                   estadoEntidadDe(jugador)
               } } };
}

MensajeSalida Juego::armarDesaparicion(uint16_t idEntidad) {
    return { TipoDestino::TODOS, 0,
             { Opcode::ENTIDAD_DESAPARECIO,
               MensajeEntidadDesaparecio{ idEntidad } } };
}


std::list<MensajeSalida> Juego::ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando) {
    bool canceloMeditacion = false;

    if (comando.opcode != Opcode::MEDITAR) {
        Jugador* jugador = buscarJugador(idCliente);
        if (jugador && jugador->enMeditacion()) {
            jugador->cancelarMeditacion();
            canceloMeditacion = true;
        }
    }

    try {
        std::list<MensajeSalida> mensajes;

        switch (comando.opcode) {
          case Opcode::MEDITAR:
            mensajes = ejecutarMeditar(idCliente);
            break;
          case Opcode::RESUCITAR:
            mensajes = ejecutarResucitar(idCliente);
            break;
          case Opcode::TOMAR:
            mensajes = ejecutarTomar(idCliente);
            break;
          case Opcode::REVISAR_CLAN:
            mensajes = ejecutarRevisarClan(idCliente);
            break;
          case Opcode::DEJAR_CLAN:
            mensajes = ejecutarDejarClan(idCliente);
            break;
          case Opcode::MOVER:
            mensajes = ejecutarMover(idCliente, std::get<ComandoMover>(comando.payload));
            break;
          case Opcode::ATACAR:
            mensajes = ejecutarAtacar(idCliente, std::get<ComandoAtacar>(comando.payload));
            break;
          case Opcode::TIRAR:
            mensajes = ejecutarTirar(idCliente, std::get<ComandoTirar>(comando.payload));
            break;
          case Opcode::EQUIPAR:
            mensajes = ejecutarEquipar(idCliente, std::get<ComandoEquipar>(comando.payload));
            break;
          case Opcode::COMPRAR:
            mensajes = ejecutarComprar(idCliente, std::get<ComandoComprar>(comando.payload));
            break;
          case Opcode::VENDER:
            mensajes = ejecutarVender(idCliente, std::get<ComandoVender>(comando.payload));
            break;
          case Opcode::DEPOSITAR_ITEM:
            mensajes = ejecutarDepositarItem(idCliente, std::get<ComandoDepositarItem>(comando.payload));
            break;
          case Opcode::DEPOSITAR_ORO:
            mensajes = ejecutarDepositarOro(idCliente, std::get<ComandoDepositarOro>(comando.payload));
            break;
          case Opcode::RETIRAR_ITEM:
            mensajes = ejecutarRetirarItem(idCliente, std::get<ComandoRetirarItem>(comando.payload));
            break;
          case Opcode::RETIRAR_ORO:
            mensajes = ejecutarRetirarOro(idCliente, std::get<ComandoRetirarOro>(comando.payload));
            break;
          case Opcode::LISTAR:
            mensajes = ejecutarListar(idCliente, std::get<ComandoListar>(comando.payload));
            break;
          case Opcode::CURAR:
            mensajes = ejecutarCurar(idCliente, std::get<ComandoCurar>(comando.payload));
            break;
          case Opcode::CHAT_GLOBAL:
            mensajes = ejecutarChatGlobal(idCliente, std::get<ComandoChatGlobal>(comando.payload));
            break;
          case Opcode::CHAT_PRIVADO:
            mensajes = ejecutarChatPrivado(idCliente, std::get<ComandoChatPrivado>(comando.payload));
            break;
          case Opcode::FUNDAR_CLAN:
            mensajes = ejecutarFundarClan(idCliente, std::get<ComandoFundarClan>(comando.payload));
            break;
          case Opcode::UNIRSE_CLAN:
            mensajes = ejecutarUnirseClan(idCliente, std::get<ComandoUnirseClan>(comando.payload));
            break;
          case Opcode::CLAN_ACEPTAR:
          case Opcode::CLAN_RECHAZAR:
          case Opcode::CLAN_BAN:
          case Opcode::CLAN_KICK:
            mensajes = ejecutarGestionMiembroClan(idCliente, std::get<ComandoGestionMiembreClan>(comando.payload), comando.opcode);
            break;
          default:
            break;
        }

        if (canceloMeditacion && comando.opcode != Opcode::MOVER) {
            if (Jugador* jugador = buscarJugador(idCliente)) {
                mensajes.push_back(armarPosicion(*jugador));
            }
        }

        return mensajes;
    } catch (const std::bad_variant_access&) {
        std::list<MensajeSalida> mensajes = {
            armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)
        };

        if (canceloMeditacion) {
            if (Jugador* jugador = buscarJugador(idCliente)) {
                mensajes.push_back(armarPosicion(*jugador));
            }
        }

        return mensajes;
    }
}

// ─── Tick del mundo ───────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::actualizar() {
    std::list<MensajeSalida> mensajes;
    for (auto& [id, jugador] : jugadoresConectados) {
        bool estabaMeditando = jugador.enMeditacion();
        jugador.recuperar(cfg.tickMs / 1000.0f);
        mensajes.push_back(armarEstado(id, jugador));

        if (estabaMeditando && !jugador.enMeditacion()) {
            mensajes.push_back(armarPosicion(jugador));
        }
    }
    // TODO: mover criaturas, aplicar aggro, respawn, expirar ítems del suelo
    return mensajes;
}

std::list<uint16_t> Juego::criaturasCerca(Posicion /*posicion*/) {
    // TODO: filtrar criaturasEnMapa por distancia al jugador cuando exista el mapa
    return {};
}

// ─── Meditar ─────────────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::ejecutarMeditar(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (!jugador->estaVivo() || !jugador->puedeMeditar())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    jugador->meditar();
    return {
        armarEstado(idCliente, *jugador),
        armarPosicion(*jugador)
    };
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

std::list<MensajeSalida> Juego::ejecutarResucitar(uint16_t idCliente) {
    // TODO: verificar que es fantasma, buscar sacerdote cercano o ciudad más cercana
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarTomar(uint16_t idCliente) {
    // TODO: verificar vivo, ítem en posición, espacio en inventario; mover del mapa al inventario
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarMover(uint16_t idCliente, const ComandoMover& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || (!jugador->estaVivo() && !jugador->esFantasma())) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    Posicion destino = jugador->getPosicion();

    switch (cmd.direccion) {
        case 0:
            if (destino.y == 0) {
                return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
            }
            destino.y--;
            break;
        case 1:
            destino.y++;
            break;
        case 2:
            if (destino.x == 0) {
                return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
            }
            destino.x--;
            break;
        case 3:
            destino.x++;
            break;
        default:
            return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    jugador->mover_a(destino.x, destino.y);
    return { armarPosicion(*jugador) };
}

std::list<MensajeSalida> Juego::ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& /*cmd*/) {
    // TODO: verificar vivo, rango, fair-play, zona segura; aplicar daño; manejar muerte
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarTirar(uint16_t idCliente, const ComandoTirar& /*cmd*/) {
    // TODO: verificar vivo, ítem en inventario, celda libre; mover del inventario al mapa
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarEquipar(uint16_t idCliente, const ComandoEquipar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!jugador->equipar_item(cmd.indiceItem, catalogo)) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    return {
        armarInventario(idCliente, *jugador),
        armarEquipamiento(idCliente, *jugador),
        armarEstado(idCliente, *jugador)
    };
}

std::list<MensajeSalida> Juego::ejecutarComprar(uint16_t idCliente, const ComandoComprar& /*cmd*/) {
    // TODO: verificar vivo, comerciante cercano, ítem disponible, oro suficiente, espacio
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarVender(uint16_t idCliente, const ComandoVender& /*cmd*/) {
    // TODO: verificar vivo, comerciante cercano, ítem en inventario
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarDepositarItem(uint16_t idCliente, const ComandoDepositarItem& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!jugador->agregar_item_banco(cmd.indiceItem)) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    return { armarInventario(idCliente, *jugador) };
}

std::list<MensajeSalida> Juego::ejecutarDepositarOro(uint16_t idCliente, const ComandoDepositarOro& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo() || cmd.monto == 0) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!jugador->agregar_oro_banco(cmd.monto)) {
        return { armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE) };
    }

    return { armarEstado(idCliente, *jugador) };
}

std::list<MensajeSalida> Juego::ejecutarRetirarItem(uint16_t idCliente, const ComandoRetirarItem& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!jugador->sacar_item_banco(cmd.idItem)) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    return { armarInventario(idCliente, *jugador) };
}

std::list<MensajeSalida> Juego::ejecutarRetirarOro(uint16_t idCliente, const ComandoRetirarOro& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo() || cmd.monto == 0) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!jugador->sacar_oro_banco(cmd.monto)) {
        return { armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE) };
    }

    return { armarEstado(idCliente, *jugador) };
}

std::list<MensajeSalida> Juego::ejecutarListar(uint16_t idCliente, const ComandoListar& /*cmd*/) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    return {{ TipoDestino::UNO, idCliente,
              { Opcode::LISTA_ITEMS, MensajeListaItems{ catalogo.idsDisponibles() } } }};
}

std::list<MensajeSalida> Juego::ejecutarCurar(uint16_t idCliente, const ComandoCurar& /*cmd*/) {
    // TODO: verificar vivo, sacerdote cercano; aplicar curación
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}
