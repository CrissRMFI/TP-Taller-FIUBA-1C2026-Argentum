#include "juego.h"

#include <list>
#include <utility>
#include <variant>
#include <optional>

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

bool mismaCelda(const Posicion& primera, const Posicion& segunda) {
  return primera.mapaId == segunda.mapaId && primera.x == segunda.x && primera.y == segunda.y;
}
}


Juego::Juego(const ConfigJuego& cfg, CatalogoItems&& cat)
    : cfg(cfg), catalogo(std::move(cat)), proximoIdClan(1) {}
    
std::list<MensajeSalida> Juego::conectarJugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion) {

    if (jugadoresConectados.find(id) != jugadoresConectados.end()) {
        return { armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    auto itNickConectado = indiceNicksConectados.find(nombre);
    if (itNickConectado != indiceNicksConectados.end() && itNickConectado->second != id) {
        return { armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

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

    indiceNicksConectados[nombre] = id;

    Jugador& jugador = jugadoresConectados.at(id);
    std::list<MensajeSalida> mensajes = {
        armarEstado(id, jugador),
        armarInventario(id, jugador),
        armarEquipamiento(id, jugador),
        armarPosicionExcepto(id, jugador)
    };

    for (const auto& [idOtro, otro] : jugadoresConectados) {
        if (idOtro != id) {
            mensajes.push_back(armarPosicionPara(id, otro));
        }
    }

    for (const ItemEnSuelo& item: mapa.obtenerItemsEnSuelo()) {
      if (item.posicion.mapaId == jugador.getPosicion().mapaId) {
        mensajes.push_back(armarItemEnSuelo(item.posicion, item.idItem));
      }
    }


    return mensajes;
}

std::list<MensajeSalida> Juego::desconectarJugador(uint16_t id) {
  auto it = jugadoresConectados.find(id);
  if (it == jugadoresConectados.end()) {
    return {};
  }

  std::list<MensajeSalida> mensajes = {
    armarDesaparicion(id)
  };
  
  const std::string nombre = it->second.getNombre();
  indiceNicksConectados.erase(nombre);
  
  jugadoresDesconectados.emplace(id, std::move(it->second));
  jugadoresConectados.erase(it);
  
  return mensajes;
}

Jugador* Juego::buscarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    return (it != jugadoresConectados.end()) ? &it->second : nullptr;
}

bool Juego::posicionOcupadaPorJugador(uint16_t idCliente, const Posicion& posicion) const {
    for (const auto& [idOtro, otro] : jugadoresConectados) {
        if (idOtro != idCliente && mismaCelda(otro.getPosicion(), posicion)) {
            return true;
        }
    }

    return false;
}

Jugador* Juego::buscarJugadorPorNick(const std::string& nick) {
    auto itIndice = indiceNicksConectados.find(nick);
    if (itIndice == indiceNicksConectados.end()) {
        return nullptr;
    }

    auto itJugador = jugadoresConectados.find(itIndice->second);
    if (itJugador == jugadoresConectados.end()) {
        return nullptr;
    }

    return &itJugador->second;
}


Clan* Juego::buscarClanPorNombre(const std::string& nombre) {
    for (auto& [id, c] : clanes)
        if (c.getNombre() == nombre) return &c;
    return nullptr;
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

MensajeSalida Juego::armarItemEnSuelo(const Posicion& posicion, uint16_t idItem) {
    return { TipoDestino::TODOS, 0, { Opcode::ITEM_EN_SUELO, MensajeItemEnSuelo{ idItem, posicion.x, posicion.y } } };
}

MensajeSalida Juego::armarItemDesaparecioSuelo(const Posicion& posicion) {
    return { TipoDestino::TODOS, 0, { Opcode::ITEM_DESAPARECIO_SUELO, MensajeItemDesaparecioSuelo{ posicion.x, posicion.y } } };
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
            mensajes = { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
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
std::list<MensajeSalida> Juego::actualizar(float deltaSegundos) {
    std::list<MensajeSalida> mensajes;

    for (auto& [id, jugador] : jugadoresConectados) {
        const bool estabaMeditando = jugador.enMeditacion();

        const uint16_t vidaAntes = jugador.getVidaActual();
        const uint16_t manaAntes = jugador.getManaActual();
        const uint32_t oroAntes = jugador.getOro();
        const uint16_t nivelAntes = jugador.getNivel();
        const uint32_t experienciaAntes = jugador.getExperiencia();

        jugador.recuperar(deltaSegundos);

        const bool cambioEstado = vidaAntes != jugador.getVidaActual() || manaAntes != jugador.getManaActual() || oroAntes != jugador.getOro() || nivelAntes != jugador.getNivel() || experienciaAntes != jugador.getExperiencia();

        if (cambioEstado) {
            mensajes.push_back(armarEstado(id, jugador));
        }

        if (estabaMeditando && !jugador.enMeditacion()) {
            mensajes.push_back(armarPosicion(jugador));

            if (!cambioEstado) {
                mensajes.push_back(armarEstado(id, jugador));
            }
        }
    }

    // TODO: mover criaturas, aplicar aggro, respawn, expirar ítems del suelo
    return mensajes;
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
    if (!jugador || !jugador->estaVivo()) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    return {{ TipoDestino::TODOS, 0,
              { Opcode::MENSAJE_CHAT, MensajeChat{ jugador->getNombre(), comando.mensaje } } }};
}

std::list<MensajeSalida> Juego::ejecutarChatPrivado(uint16_t idCliente, const ComandoChatPrivado& comando) {
    Jugador* emisor = buscarJugador(idCliente);
    if (!emisor || !emisor->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    auto itDestino = indiceNicksConectados.find(comando.nickDestino);
    if (itDestino == indiceNicksConectados.end()) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    return {{ TipoDestino::UNO, itDestino->second, { Opcode::MENSAJE_CHAT, MensajeChat{ emisor->getNombre(), comando.mensaje } } }};
}

// ─── Clan ─────────────────────────────────────────────────────────────────────

std::list<MensajeSalida> Juego::ejecutarFundarClan(uint16_t idCliente, const ComandoFundarClan& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (!jugador->estaVivo() || jugador->tieneClan()) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }
    
    if (jugador->getNivel() < cfg.clanNivelMinimo) {
      return { armarError(idCliente, CodigoErrorAccion::NIVEL_INSUFICIENTE) };
    }
        
    if (buscarClanPorNombre(comando.nombreClan)) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }
        

    uint16_t idClan = proximoIdClan++;
    clanes.emplace(idClan, Clan(idClan, comando.nombreClan, jugador->getNombre()));
    jugador->asignarClan(idClan);
    jugador->marcarFundadorClan();

    return {{ TipoDestino::UNO, idCliente,
              { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Fundado, comando.nombreClan } } }};
}

std::list<MensajeSalida> Juego::ejecutarUnirseClan(uint16_t idCliente, const ComandoUnirseClan& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (!jugador->estaVivo() || jugador->tieneClan()) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }
    
    Clan* clan = buscarClanPorNombre(comando.nombreClan);
    if (!clan) {
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }
        
    const std::string nickSolicitante = jugador->getNombre();
    
    if (clan->estaBaneado(nickSolicitante))
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    
        
    if ((int)(clan->cantidadMiembros()) >= cfg.clanMaxMiembros) {
      return { armarError(idCliente, CodigoErrorAccion::CLAN_LLENO) };
    }
        

    clan->pedirUnirse(nickSolicitante);

    // Notificar al fundador si está conectado
    std::list<MensajeSalida> mensajes;
    uint16_t idClan = clan->getId();
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

    if (!jugador || !jugador->estaVivo() || !jugador->tieneClan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    if (jugador->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    clanes.at(jugador->getClan()).eliminarMiembro(jugador->getNombre());
    jugador->salirClan();
    return {};
}

std::list<MensajeSalida> Juego::ejecutarRevisarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo() || !jugador->tieneClan() || !jugador->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    Clan& clan = clanes.at(jugador->getClan());
    std::list<MensajeSalida> mensajes;

    for (const std::string& nickPendiente: clan.obtenerPendientes()) {
      mensajes.push_back({ TipoDestino::UNO, idCliente, { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::MiembroPendiente, nickPendiente } } });
    }
    
    for (const std::string& nickMiembro: clan.obtenerMiembros()) {
      mensajes.push_back({ TipoDestino::UNO, idCliente, { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::MiembroActivo, nickMiembro } } });
    }

    return mensajes;
}

std::list<MensajeSalida> Juego::ejecutarGestionMiembroClan(uint16_t idCliente, const ComandoGestionMiembreClan& comando, Opcode accion) {
    
    Jugador* lider = buscarJugador(idCliente);
    if (!lider || !lider->estaVivo() || !lider->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    Clan& clan = clanes.at(lider->getClan());
    Jugador* objetivo = buscarJugadorPorNick(comando.nick);

    // Para operaciones sobre miembros activos/pendientes el objetivo puede estar desconectado.
    // Buscamos su id iterando ambos mapas.
    uint16_t idObjetivo = 0;
    
    auto itNickConectado = indiceNicksConectados.find(comando.nick);
    
    if (itNickConectado != indiceNicksConectados.end()) {
      idObjetivo = itNickConectado->second;
    }
    
    if (!idObjetivo) {
    for (auto& [id, j] : jugadoresDesconectados) {
        if (j.getNombre() == comando.nick) {
            idObjetivo = id;
            break;
        }
    }
}
    if (!idObjetivo)
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };

    if (idObjetivo == idCliente)
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    if ((accion == Opcode::CLAN_KICK || accion == Opcode::CLAN_BAN) &&
    clan.esFundador(comando.nick)) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    std::list<MensajeSalida> msgs;

    switch (accion) {
      case Opcode::CLAN_ACEPTAR: {
        if (!clan.estaPendiente(comando.nick)) {
          return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        }
            
        clan.agregarMiembro(comando.nick);
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
        if (!clan.estaPendiente(comando.nick)) {
          return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        }
            
        clan.eliminarMiembro(comando.nick);
        if (objetivo)
            msgs.push_back({ TipoDestino::UNO, idObjetivo,
                { Opcode::MENSAJE_CLAN, MensajeClan{ TipoMensajeClan::Rechazado, clan.getNombre() } } });
        break;

      case Opcode::CLAN_BAN:
        if (!clan.esMiembro(comando.nick) && !clan.estaPendiente(comando.nick)) {
          return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        }
            
        clan.banearMiembro(comando.nick);
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
        if (!clan.esMiembro(comando.nick)) {
          return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        }
            
        clan.eliminarMiembro(comando.nick);
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
  
  Jugador* jugador = buscarJugador(idCliente);
  
  if (!jugador || !jugador->estaVivo()) {
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
  }
  
  Posicion posicion = jugador->getPosicion();
  std::optional<uint16_t> idItem = mapa.tomarItem(posicion);
  
  if (!idItem.has_value()) {
    return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
  }
  
  if (!catalogo.existe(*idItem)) {
    mapa.agregarItem(posicion, *idItem);
    return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
  }
  
  if (!jugador->agregar_item(*idItem)) {
    mapa.agregarItem(posicion, *idItem);
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
  }
  
  return {
    armarInventario(idCliente, *jugador),
    armarItemDesaparecioSuelo(posicion)
  };
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

    if (!mapa.posicionValida(destino) || mapa.hayParedEn(destino) || mapa.hayNpcEn(destino)) {
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }
    
    if (posicionOcupadaPorJugador(idCliente, destino)) {
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }
    
    jugador->mover_a(destino.x, destino.y);
    
    return { 
      armarPosicion(*jugador) 
    };

    // TODO: validar destino contra Mapa cuando existan límites, paredes,
    // NPCs, criaturas, ciudades y zonas seguras.
    jugador->mover_a(destino.x, destino.y);

    return { armarPosicion(*jugador) };
}

std::list<MensajeSalida> Juego::ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& /*cmd*/) {
    // TODO: implementar combate cuando exista Mapa/NPC/objetivos.
    //
    // Reglas pendientes del enunciado:
    // - Validar que el atacante exista y esté vivo.
    // - Buscar objetivo jugador o criatura por id.
    // - Validar rango cuerpo a cuerpo o distancia según arma/hechizo.
    // - Bloquear ataques en zonas seguras.
    // - Aplicar protección newbie: nivel <= cfg.nivelNewbie.
    // - Aplicar regla PVP: |nivelAtacante - nivelDefensor| <= cfg.maxDiffNivel.
    // - Bloquear fuego amigo entre miembros del mismo clan.
    // - Calcular crítico con cfg.probabilidadCritico.
    // - Si es crítico, duplicar daño y omitir evasión.
    // - Si no es crítico, aplicar evasión con cfg.esquivarUmbral.
    // - Aplicar absorción por armadura/casco/escudo.
    // - Otorgar XP por impacto y por kill.
    // - Manejar muerte, drops de oro/items y estado fantasma.
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarTirar(uint16_t idCliente, const ComandoTirar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    
    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    Posicion posicion = jugador->getPosicion();

    if (mapa.hayItemEn(posicion)) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    uint16_t idItem = jugador->quitar_item_de_slot(cmd.indiceItem);
    
    if (idItem == 0) {
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }
    
    if (!catalogo.existe(idItem)) {
      jugador->agregar_item(idItem);
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }
    
    if (!mapa.agregarItem(posicion, idItem)) {
      jugador->agregar_item(idItem);
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }
    
    return {
      armarInventario(idCliente, *jugador),
      armarItemEnSuelo(posicion, idItem)
    };
  
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

std::list<MensajeSalida> Juego::ejecutarDepositarItem(uint16_t idCliente,
                                                      const ComandoDepositarItem& /*cmd*/) {
    // TODO: validar jugador vivo, cercanía a banquero e ítem disponible.
    // El enunciado define estas operaciones como interacción con NPC Banquero.
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarDepositarOro(uint16_t idCliente, const ComandoDepositarOro& /*cmd*/) {
    
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!mapa.hayNpcCercano(jugador->getPosicion(), TipoNpc::Banquero)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    // TODO: implementar depósito cuando esté definido el flujo de banco.
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarRetirarItem(uint16_t idCliente,
                                                    const ComandoRetirarItem& /*cmd*/) {
    // TODO: validar jugador vivo, cercanía a banquero, ítem depositado y espacio en inventario.
    // El enunciado define estas operaciones como interacción con NPC Banquero.
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarRetirarOro(uint16_t idCliente,
                                                   const ComandoRetirarOro& /*cmd*/) {
    // TODO: validar jugador vivo, cercanía a banquero, monto positivo y oro depositado.
    // El enunciado define estas operaciones como interacción con NPC Banquero.
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarListar(uint16_t idCliente,
                                               const ComandoListar& /*cmd*/) {
    // TODO: validar jugador vivo y cercanía a comerciante o sacerdote.
    // El listado de ítems disponibles debe depender del NPC con el que se interactúa.
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<MensajeSalida> Juego::ejecutarCurar(uint16_t idCliente, const ComandoCurar& /*cmd*/) {
    // TODO: verificar vivo, sacerdote cercano; aplicar curación
    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}
