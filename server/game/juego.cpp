#include "juego.h"

#include <list>
#include <utility>
#include <variant>
#include <optional>
#include <random>
#include <vector>
#include <cstdlib>
#include "objeto/catalogo_items.h"
#include "../../common/protocolo/tipo_entidad.h"

namespace {
uint8_t estadoEntidadDe(const Jugador& jugador) {
    if (jugador.getEstado() == Estado::Vivo) {
        return 0;
    }

    if (jugador.getEstado() == Estado::Fantasma) {
        return 1;
    }

    if (jugador.getEstado() == Estado::Meditando) {
        return 2;
    }

    return 0;
}

}

Juego::Juego(const ConfigJuego& cfg, CatalogoItems&& cat) : cfg(cfg), catalogo(std::move(cat)), proximoIdClan(1), mapa(cfg.mapaAncho, cfg.mapaAlto), ticksTranscurridos(0) {}

std::list<EventoSalida> Juego::conectarJugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion) {

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
        jugadoresConectados.emplace(id, Jugador(id, nombre, clase, raza, posicion, &cfg));
    }

    indiceNicksConectados[nombre] = id;

    Jugador& jugador = jugadoresConectados.at(id);
    std::list<EventoSalida> mensajes = {
        armarEstado(id, jugador),
        armarInventario(id, jugador),
        armarEquipamiento(id, jugador)
    };

    mensajes.splice(mensajes.end(), armarPosicionParaMapa(jugador));

    for (const auto& [idOtro, otro] : jugadoresConectados) {
        if (idOtro != id && otro.getPosicion().mapaId == jugador.getPosicion().mapaId) {
            mensajes.push_back(armarPosicionPara(id, otro));
        }
    }

    for (const ItemEnSuelo& item: mapa.obtenerItemsEnSuelo()) {
        if (item.posicion.mapaId == jugador.getPosicion().mapaId) {
            mensajes.push_back({ TipoDestino::UNO, id,
                                 EventoItemEnSuelo{
                                     item.idItem,
                                     item.posicion.x,
                                     item.posicion.y } });
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::desconectarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    if (it == jugadoresConectados.end()) {
        return {};
    }

    std::list<EventoSalida> mensajes = armarDesaparicionParaMapa(it->second);

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

std::optional<uint16_t> Juego::buscarIdJugadorEn(
        const Posicion& posicion,
        std::optional<uint16_t> idExcluido) const {
    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (idExcluido.has_value() && idCliente == *idExcluido) {
            continue;
        }

        if (jugador.getPosicion() == posicion) {
            return idCliente;
        }
    }

    return std::nullopt;
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

EventoSalida Juego::armarError(uint16_t idCliente, CodigoErrorAccion cod) {
    return { TipoDestino::UNO, idCliente, EventoErrorAccion{ cod } };
}

EventoSalida Juego::armarEstado(uint16_t idCliente, const Jugador& jugador) {
    return { TipoDestino::UNO, idCliente,
             EventoEstadoPersonaje{
                 jugador.getVidaActual(), jugador.getVidaMax(),
                 jugador.getManaActual(), jugador.getManaMax(),
                 jugador.getOro(), jugador.getNivel(), jugador.getExperiencia()
             } };
}

EventoSalida Juego::armarInventario(uint16_t idCliente, const Jugador& jugador) {
    return { TipoDestino::UNO, idCliente,
             EventoActualizarInventario{ jugador.getSlotsInventario() } };
}

EventoSalida Juego::armarEquipamiento(uint16_t idCliente, const Jugador& jugador) {
    return { TipoDestino::UNO, idCliente,
             EventoActualizarEquipamiento{
                 jugador.getArmaEquipada(),
                 jugador.getBaculoEquipado(),
                 jugador.getDefensaEquipada(),
                 jugador.getCascoEquipado(),
                 jugador.getEscudoEquipado()
             } };
}

EventoSalida Juego::armarPosicionPara(uint16_t idCliente, const Jugador& jugador) {
    Posicion posicion = jugador.getPosicion();
    return { TipoDestino::UNO, idCliente,
             EventoPosicionEntidad{
                 jugador.getId(),
                 posicion.x,
                 posicion.y,
                 static_cast<uint8_t>(TipoEntidad::Personaje),
                 estadoEntidadDe(jugador)
             } };
}

std::list<EventoSalida> Juego::armarPosicionParaMapa(const Jugador& jugador) {
    std::list<EventoSalida> mensajes;
    Posicion posicion = jugador.getPosicion();

    for (const auto& [idCliente, otro]: jugadoresConectados) {
        if (otro.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back(armarPosicionPara(idCliente, jugador));
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarDesaparicionParaMapa(const Jugador& jugador) {
    std::list<EventoSalida> mensajes;
    Posicion posicion = jugador.getPosicion();

    for (const auto& [idCliente, otro]: jugadoresConectados) {
        if (idCliente != jugador.getId() &&
            otro.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back({ TipoDestino::UNO, idCliente,
                                 EventoEntidadDesaparecio{ jugador.getId() } });
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarItemEnSueloParaMapa(const Posicion& posicion, uint16_t idItem) {
    std::list<EventoSalida> mensajes;

    for (const auto& [idCliente, jugador]: jugadoresConectados) {
      if (jugador.getPosicion().mapaId == posicion.mapaId) {
        mensajes.push_back({ TipoDestino::UNO, idCliente,
                             EventoItemEnSuelo{ idItem, posicion.x, posicion.y } });
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarItemDesaparecioSueloParaMapa(const Posicion& posicion) {
    std::list<EventoSalida> mensajes;

    for (const auto& [idCliente, jugador]: jugadoresConectados) {
      if (jugador.getPosicion().mapaId == posicion.mapaId) {
        mensajes.push_back({ TipoDestino::UNO, idCliente,
                             EventoItemDesaparecioSuelo{ posicion.x, posicion.y } });
        }
    }

    return mensajes;
}

bool Juego::agregarItemEnSueloCercano(const Posicion& origen, uint16_t idItem, Posicion& posicionFinal) {
    if (mapa.agregarItem(origen, idItem)) {
        posicionFinal = origen;
        return true;
    }

    for (const Posicion& posicionCandidata : calcularDestinosAdyacentes(origen)) {
        if (mapa.agregarItem(posicionCandidata, idItem)) {
            posicionFinal = posicionCandidata;
            return true;
        }
    }

    return false;
}

std::list<EventoSalida> Juego::ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando) {
    bool canceloMeditacion = false;

    if (comando.opcode != Opcode::MEDITAR) {
        Jugador* jugador = buscarJugador(idCliente);
        if (jugador && jugador->enMeditacion()) {
            jugador->cancelarMeditacion();
            canceloMeditacion = true;
        }
    }

    try {
        std::list<EventoSalida> mensajes;

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
                std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(*jugador);
                mensajes.splice(mensajes.end(), mensajesPosicion);
            }
        }

        return mensajes;
    } catch (const std::bad_variant_access&) {
        std::list<EventoSalida> mensajes = {
            armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)
        };

        if (canceloMeditacion) {
            if (Jugador* jugador = buscarJugador(idCliente)) {
                std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(*jugador);
                mensajes.splice(mensajes.end(), mensajesPosicion);
            }
        }

        return mensajes;
    }
}

// ─── Tick del mundo ───────────────────────────────────────────────────────────
std::list<EventoSalida> Juego::actualizar(float deltaSegundos) {
    std::list<EventoSalida> mensajes;
    ticksTranscurridos++;

    for (auto& [id, jugador] : jugadoresConectados) {
        const bool estabaMeditando = jugador.enMeditacion();
        const bool estabaInmovilizado = jugador.estaInmovilizado();

        const uint16_t vidaAntes = jugador.getVidaActual();
        const uint16_t manaAntes = jugador.getManaActual();
        const uint32_t oroAntes = jugador.getOro();
        const uint16_t nivelAntes = jugador.getNivel();
        const uint32_t experienciaAntes = jugador.getExperiencia();

        jugador.recuperar(deltaSegundos);

        const bool cambioEstado =
            vidaAntes != jugador.getVidaActual() ||
            manaAntes != jugador.getManaActual() ||
            oroAntes != jugador.getOro() ||
            nivelAntes != jugador.getNivel() ||
            experienciaAntes != jugador.getExperiencia();

        if (cambioEstado) {
            mensajes.push_back(armarEstado(id, jugador));
        }
        if (estabaInmovilizado && !jugador.estaInmovilizado()) {
            Posicion posicionResurreccion = jugador.getPosicionResurreccion();
            std::optional<Posicion> posicionResurreccionCercana = mapa.obtenerPosicionResurreccionCercana(posicionResurreccion);
            if (!posicionResurreccionCercana.has_value()) {
                // Intentar revivirlo en el proximo tick, manteniendolo inmovilizado
                jugador.inmovilizar(posicionResurreccion.x, posicionResurreccion.y, deltaSegundos);

            } else {
                jugador.resucitar(posicionResurreccionCercana->x, posicionResurreccionCercana->y);
                std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(jugador);
                mensajes.splice(mensajes.end(), mensajesPosicion);

                if (!cambioEstado) {
                    mensajes.push_back(armarEstado(id, jugador));
                }
            }
        } else if (estabaMeditando && !jugador.enMeditacion()) {
            std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(jugador);
            mensajes.splice(mensajes.end(), mensajesPosicion);

            if (!cambioEstado) {
                mensajes.push_back(armarEstado(id, jugador));
            }
        }
    }

    if (cfg.movimientoCriaturasTicks > 0 && ticksTranscurridos % cfg.movimientoCriaturasTicks == 0) {
      std::list<EventoSalida> mensajesCriaturas = actualizarCriaturas();
      mensajes.splice(mensajes.end(), mensajesCriaturas);
    }

    std::vector<ItemEnSuelo> itemsExpirados =
            mapa.actualizarItemsEnSuelo(deltaSegundos, cfg.tiempoItemSueloSeg);

    for (const ItemEnSuelo& item : itemsExpirados) {
        std::list<EventoSalida> mensajesItem = armarItemDesaparecioSueloParaMapa(item.posicion);
        mensajes.splice(mensajes.end(), mensajesItem);
    }

    return mensajes;
}

// ─── Meditar ─────────────────────────────────────────────────────────────────

std::list<EventoSalida> Juego::ejecutarMeditar(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) return {};

    if (!jugador->estaVivo() || !jugador->puedeMeditar())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    jugador->meditar();

    std::list<EventoSalida> mensajes = {
        armarEstado(idCliente, *jugador)
    };

    mensajes.splice(mensajes.end(), armarPosicionParaMapa(*jugador));
    return mensajes;
}

// ─── Chat ─────────────────────────────────────────────────────────────────────

std::list<EventoSalida> Juego::ejecutarChatGlobal(uint16_t idCliente, const ComandoChatGlobal& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    return {{ TipoDestino::TODOS, 0,
              EventoChat{ jugador->getNombre(), comando.mensaje } }};
}

std::list<EventoSalida> Juego::ejecutarChatPrivado(uint16_t idCliente, const ComandoChatPrivado& comando) {
    Jugador* emisor = buscarJugador(idCliente);
    if (!emisor || !emisor->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    auto itDestino = indiceNicksConectados.find(comando.nickDestino);
    if (itDestino == indiceNicksConectados.end()) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    return {{ TipoDestino::UNO, itDestino->second,
              EventoChat{ emisor->getNombre(), comando.mensaje } }};
}

// ─── Clan ─────────────────────────────────────────────────────────────────────

std::list<EventoSalida> Juego::ejecutarFundarClan(uint16_t idCliente, const ComandoFundarClan& comando) {
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
              EventoClan{ TipoEventoClan::Fundado, comando.nombreClan } }};
}

std::list<EventoSalida> Juego::ejecutarUnirseClan(uint16_t idCliente, const ComandoUnirseClan& comando) {
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
    std::list<EventoSalida> mensajes;
    uint16_t idClan = clan->getId();
    for (auto& [id, jugador] : jugadoresConectados) {
        if (jugador.fundo_clan() && jugador.getClan() == idClan) {
            mensajes.push_back({ TipoDestino::UNO, id,
                EventoClan{ TipoEventoClan::MiembroPendiente, nickSolicitante } });
        }
    }
    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarDejarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo() || !jugador->tieneClan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    if (jugador->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    clanes.at(jugador->getClan()).eliminarMiembro(jugador->getNombre());
    jugador->salirClan();
    return {};
}

std::list<EventoSalida> Juego::ejecutarRevisarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo() || !jugador->tieneClan() || !jugador->fundo_clan())
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };

    Clan& clan = clanes.at(jugador->getClan());
    std::list<EventoSalida> mensajes;

    for (const std::string& nickPendiente: clan.obtenerPendientes()) {
      mensajes.push_back({ TipoDestino::UNO, idCliente,
                           EventoClan{ TipoEventoClan::MiembroPendiente, nickPendiente } });
    }

    for (const std::string& nickMiembro: clan.obtenerMiembros()) {
      mensajes.push_back({ TipoDestino::UNO, idCliente,
                           EventoClan{ TipoEventoClan::MiembroActivo, nickMiembro } });
    }

    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarGestionMiembroClan(uint16_t idCliente, const ComandoGestionMiembreClan& comando, Opcode accion) {
    
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

    std::list<EventoSalida> msgs;

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
                EventoClan{ TipoEventoClan::Aceptado, clan.getNombre() } });
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
                EventoClan{ TipoEventoClan::Rechazado, clan.getNombre() } });
        break;

      case Opcode::CLAN_BAN:
        if (!clan.esMiembro(comando.nick) && !clan.estaPendiente(comando.nick)) {
          return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
        }

        clan.banearMiembro(comando.nick);
        if (objetivo) {
            objetivo->salirClan();
            msgs.push_back({ TipoDestino::UNO, idObjetivo,
                EventoClan{ TipoEventoClan::Baneado, clan.getNombre() } });
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
                EventoClan{ TipoEventoClan::Kickeado, clan.getNombre() } });
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

std::list<EventoSalida> Juego::ejecutarResucitar(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->esFantasma() || jugador->estaInmovilizado()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    // Buscar al sacerdote más cercano en el mapa del jugador.
    std::optional<Npc> npcSacerdote = mapa.buscarSacerdoteMasCercano(jugador->getPosicion());
    if (!npcSacerdote.has_value()) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    // Validar via el accessor con puntero observador que el sacerdote
    // efectivamente exista como entidad concreta en el mapa.
    const Sacerdote* sacerdote = mapa.obtenerSacerdote(npcSacerdote->getId());
    if (sacerdote == nullptr) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    const int distancia = jugador->getPosicion().distanciaManhattan(sacerdote->getPosicion());
    if (distancia <= cfg.rangoInteraccionNpc) {
        // Resucitar al jugador en una celda libre adyacente a su posición actual.
        std::optional<Posicion> posicionResurreccion = mapa.obtenerPosicionResurreccionCercana(jugador->getPosicion());
        if (!posicionResurreccion.has_value()) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        jugador->resucitar(posicionResurreccion->x, posicionResurreccion->y);

        std::list<EventoSalida> mensajes = {
            armarEstado(idCliente, *jugador)
        };

        mensajes.splice(mensajes.end(), armarPosicionParaMapa(*jugador));
        return mensajes;
    } else {
        // Revivir al jugador junto al sacerdote de la ciudad mas proxima.
        std::optional<Posicion> posicionResurreccion = mapa.obtenerPosicionResurreccionCercana(sacerdote->getPosicion());
        if (!posicionResurreccion.has_value()) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        float tiempoInmovilizado = distancia * cfg.factorTiempoResurreccion;

        jugador->inmovilizar(posicionResurreccion->x, posicionResurreccion->y, tiempoInmovilizado);
        
        return {
            armarEstado(idCliente, *jugador),
        };
    }
}

std::list<EventoSalida> Juego::ejecutarTomar(uint16_t idCliente) {
  
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
  
  std::list<EventoSalida> mensajes = {
    armarInventario(idCliente, *jugador)
  };

  mensajes.splice(mensajes.end(), armarItemDesaparecioSueloParaMapa(posicion));

  return mensajes;
}

std::list<EventoSalida> Juego::ejecutarMover(uint16_t idCliente, const ComandoMover& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || (!jugador->estaVivo() && !jugador->esFantasma()) || jugador->estaInmovilizado()) {
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

    // Colisión absoluta: una celda no puede ser compartida por dos entidades.
    // Aplica tanto a jugadores vivos como a fantasmas — el enunciado no
    // diferencia el modelo de colisión por estado del personaje.
    if (!mapa.posicionValida(destino) || mapa.hayParedEn(destino) ||
        mapa.hayNpcEn(destino) || mapa.hayCriaturaEn(destino)) {
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    if (buscarIdJugadorEn(destino, idCliente).has_value()) {
      return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    jugador->mover_a(destino.x, destino.y);
    
    return armarPosicionParaMapa(*jugador);
}

std::list<EventoSalida> Juego::ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& cmd) {
    Jugador* atacante = buscarJugador(idCliente);

    if (!atacante || !atacante->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (cmd.idObjetivo == idCliente) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    Jugador* objetivo = buscarJugador(cmd.idObjetivo);

    if (!objetivo || !objetivo->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    const Posicion posicionAtacante = atacante->getPosicion();
    const Posicion posicionObjetivo = objetivo->getPosicion();

    if (!posicionAtacante.mismaMapa(posicionObjetivo)) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    if (mapa.esZonaSegura(posicionAtacante) || mapa.esZonaSegura(posicionObjetivo)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (atacante->es_newbie() || objetivo->es_newbie()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const int diferenciaNivel =
            std::abs(static_cast<int>(atacante->getNivel()) - static_cast<int>(objetivo->getNivel()));

    if (diferenciaNivel > cfg.maxDiffNivel) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (atacante->tieneClan() && objetivo->tieneClan() &&
            atacante->getClan() == objetivo->getClan()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!posicionAtacante.esAdyacente(posicionObjetivo)) {
        return { armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO) };
    }

    const uint16_t danioBruto = atacante->calcular_danio(catalogo);
    const uint16_t danioAplicado = objetivo->recibir_ataque_fisico(danioBruto, catalogo);

    std::list<EventoSalida> mensajes;

    mensajes.push_back(EventoSalida{
        TipoDestino::UNO, idCliente,
        EventoDanioProducido{ danioAplicado, objetivo->getId() }
    });

    mensajes.push_back(EventoSalida{
        TipoDestino::UNO, objetivo->getId(),
        EventoDanioRecibido{ danioAplicado, atacante->getId() }
    });

    mensajes.push_back(armarEstado(objetivo->getId(), *objetivo));

    if (!objetivo->estaVivo()) {
        const EventoMuerteEntidad eventoMuerte{ objetivo->getId() };
        const Posicion posicionObjetivo = objetivo->getPosicion();

        for (const auto& [idOtro, otroJugador] : jugadoresConectados) {
            if (otroJugador.getPosicion().mapaId == posicionObjetivo.mapaId) {
                mensajes.push_back(EventoSalida{
                    TipoDestino::UNO, idOtro, eventoMuerte
                });
            }
        }

        const std::vector<uint16_t> itemsDropear = objetivo->vaciar_inventario();

        for (uint16_t idItem : itemsDropear) {
            Posicion posicionDrop = posicionObjetivo;
            if (agregarItemEnSueloCercano(posicionObjetivo, idItem, posicionDrop)) {
                mensajes.splice(mensajes.end(), armarItemEnSueloParaMapa(posicionDrop, idItem));
            }
        }

        mensajes.push_back(armarInventario(objetivo->getId(), *objetivo));

        std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(*objetivo);
        mensajes.splice(mensajes.end(), mensajesPosicion);
    }
    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarTirar(uint16_t idCliente, const ComandoTirar& cmd) {
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
    
    std::list<EventoSalida> mensajes = {
      armarInventario(idCliente, *jugador)
    };

    mensajes.splice(mensajes.end(), armarItemEnSueloParaMapa(posicion, idItem));
    return mensajes;

  }

std::list<EventoSalida> Juego::ejecutarEquipar(uint16_t idCliente, const ComandoEquipar& cmd) {
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

std::list<EventoSalida> Juego::ejecutarComprar(uint16_t idCliente, const ComandoComprar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const auto npcComerciante = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Comerciante, cfg.rangoInteraccionNpc);
    const auto npcSacerdote = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Sacerdote, cfg.rangoInteraccionNpc);

    if (npcComerciante.has_value()) {
        Comerciante* comerciante = mapa.obtenerComerciante(npcComerciante->getId());

        if (comerciante == nullptr) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        std::pair<bool, uint8_t> resultado = comerciante->venderItem(cmd.idItem);
        bool puedeComprar = resultado.first;

        if (!puedeComprar) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        uint8_t precioCompra = resultado.second;

        if (jugador->getOro() < precioCompra) {
            return { armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE) };
        }

        if (!jugador->agregar_item(cmd.idItem)) {
            return { armarError(idCliente, CodigoErrorAccion::INVENTARIO_LLENO) };
        }

        jugador->gastar_oro(precioCompra);

        return {
            armarInventario(idCliente, *jugador),
            armarEstado(idCliente, *jugador)
        };
        
    } else if (npcSacerdote.has_value()) {
        Sacerdote* sacerdote = mapa.obtenerSacerdote(npcSacerdote->getId());

        if (sacerdote == nullptr) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        std::pair<bool, uint8_t> resultado = sacerdote->venderItem(cmd.idItem);
        bool puedeComprar = resultado.first;

        if (!puedeComprar) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        uint8_t precioCompra = resultado.second;

        if (jugador->getOro() < precioCompra) {
            return { armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE) };
        }

        if (!jugador->agregar_item(cmd.idItem)) {
            return { armarError(idCliente, CodigoErrorAccion::INVENTARIO_LLENO) };
        }

        jugador->gastar_oro(precioCompra);

        return {
            armarInventario(idCliente, *jugador),
            armarEstado(idCliente, *jugador)
        };
    }

    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<EventoSalida> Juego::ejecutarVender(uint16_t idCliente, const ComandoVender& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const auto npcComerciante = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Comerciante, cfg.rangoInteraccionNpc);
    if (!npcComerciante.has_value()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    uint16_t idItem = jugador->quitar_item_de_slot(cmd.indiceItem);
    Comerciante* comerciante = mapa.obtenerComerciante(npcComerciante->getId());

    if (comerciante == nullptr) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    std::pair<bool, uint8_t> resultado = comerciante->comprarItem(idItem);
    bool puedeVender = resultado.first;

    if (!puedeVender) {
        if (catalogo.existe(idItem)) {
            jugador->agregar_item_en_slot(idItem, cmd.indiceItem);
        }
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    uint8_t precioVenta = resultado.second;

    jugador->sumar_oro(precioVenta);

    return {
        armarInventario(idCliente, *jugador),
        armarEstado(idCliente, *jugador)
    };
}

std::list<EventoSalida> Juego::ejecutarDepositarItem(uint16_t idCliente,
                                                      const ComandoDepositarItem& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const auto npcBanquero = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Banquero, cfg.rangoInteraccionNpc);
    if (!npcBanquero.has_value()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    Banquero* banquero = mapa.obtenerBanquero(npcBanquero->getId());

    if (banquero == nullptr) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    uint16_t idItem = jugador->quitar_item_de_slot(cmd.indiceItem);

    if (!banquero->depositarItem(idCliente, idItem)) {
        if (catalogo.existe(idItem)) {
            jugador->agregar_item_en_slot(idItem, cmd.indiceItem);
        }
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }
    return { armarInventario(idCliente, *jugador) };
}

std::list<EventoSalida> Juego::ejecutarDepositarOro(uint16_t idCliente, const ComandoDepositarOro& cmd) {
    
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
      return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const auto npcBanquero = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Banquero, cfg.rangoInteraccionNpc);
    if (!npcBanquero.has_value()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    Banquero* banquero = mapa.obtenerBanquero(npcBanquero->getId());

    if (banquero == nullptr) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (jugador->getOro() < cmd.monto) {
        return { armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE) };
    }

    if (!banquero->depositarOro(idCliente, cmd.monto)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    jugador->gastar_oro(cmd.monto);

    return { armarInventario(idCliente, *jugador) };
}

std::list<EventoSalida> Juego::ejecutarRetirarItem(uint16_t idCliente,
                                                    const ComandoRetirarItem& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const auto npcBanquero = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Banquero, cfg.rangoInteraccionNpc);
    if (!npcBanquero.has_value()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    Banquero* banquero = mapa.obtenerBanquero(npcBanquero->getId());

    if (banquero == nullptr) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!banquero->retirarItem(idCliente, cmd.idItem)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!jugador->agregar_item(cmd.idItem)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    return { armarInventario(idCliente, *jugador) };

}

std::list<EventoSalida> Juego::ejecutarRetirarOro(uint16_t idCliente,
                                                   const ComandoRetirarOro& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const auto npcBanquero = mapa.buscarNpcCercano(jugador->getPosicion(), TipoNpc::Banquero, cfg.rangoInteraccionNpc);
    if (!npcBanquero.has_value()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    Banquero* banquero = mapa.obtenerBanquero(npcBanquero->getId());

    if (banquero == nullptr) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!banquero->retirarOro(idCliente, cmd.monto)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    jugador->sumar_oro(cmd.monto);

    return { armarInventario(idCliente, *jugador) };
}

std::list<EventoSalida> Juego::ejecutarListar(uint16_t idCliente,
                                               const ComandoListar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    const Posicion posicionJugador = jugador->getPosicion();

    if (Comerciante* comerciante = mapa.obtenerComerciante(cmd.idNPC)) {
        const Posicion posicionNpc = comerciante->getPosicion();
        if (!posicionJugador.mismaMapa(posicionNpc) ||
                posicionJugador.distanciaManhattan(posicionNpc) > cfg.rangoInteraccionNpc) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        std::vector<uint16_t> ids;
        for (const auto& [idItem, precios] : comerciante->listarItemsDisponibles()) {
            ids.push_back(idItem);
        }

        return { EventoSalida{ TipoDestino::UNO, idCliente, EventoListaItems{ ids } } };
    }

    if (Sacerdote* sacerdote = mapa.obtenerSacerdote(cmd.idNPC)) {
        const Posicion posicionNpc = sacerdote->getPosicion();
        if (!posicionJugador.mismaMapa(posicionNpc) ||
                posicionJugador.distanciaManhattan(posicionNpc) > cfg.rangoInteraccionNpc) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        std::vector<uint16_t> ids;
        for (const auto& [idItem, precio] : sacerdote->listarItemsDisponibles()) {
            ids.push_back(idItem);
        }

        return { EventoSalida{ TipoDestino::UNO, idCliente, EventoListaItems{ ids } } };
    }

    if (Banquero* banquero = mapa.obtenerBanquero(cmd.idNPC)) {
        const Posicion posicionNpc = banquero->getPosicion();
        if (!posicionJugador.mismaMapa(posicionNpc) ||
                posicionJugador.distanciaManhattan(posicionNpc) > cfg.rangoInteraccionNpc) {
            return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
        }

        std::pair<uint32_t, std::vector<uint16_t>> cuenta =
                banquero->listarItemsDisponibles(idCliente);
        return { EventoSalida{ TipoDestino::UNO, idCliente, EventoListaItems{ cuenta.second } } };
    }

    return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
}

std::list<EventoSalida> Juego::ejecutarCurar(uint16_t idCliente, const ComandoCurar& /*cmd*/) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }

    if (!mapa.hayNpcCercano(jugador->getPosicion(), TipoNpc::Sacerdote, cfg.rangoInteraccionNpc)) {
        return { armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA) };
    }
    jugador->curar(jugador->getVidaMax());
    jugador->recuperar_mana(jugador->getManaMax());
    return { armarEstado(idCliente, *jugador) };
}

std::list<EventoSalida> Juego::actualizarCriaturas() {
    std::list<EventoSalida> mensajes;
    std::vector<uint16_t> idsCriaturas;

    for (const Criatura& criatura : mapa.obtenerCriaturas()) {
        idsCriaturas.push_back(criatura.getId());
    }

    for (uint16_t idCriatura : idsCriaturas) {
        Criatura* criatura = mapa.obtenerCriaturaPor(idCriatura);
        if (criatura == nullptr) {
            continue;
        }

        std::optional<Jugador> jugadorCercano = buscarJugadorCercano(*criatura);

        if (jugadorCercano.has_value()) {
            std::list<EventoSalida> mensajesAtaque =
                moverCriaturaHacia(*criatura, jugadorCercano->getPosicion());

            mensajes.splice(mensajes.end(), mensajesAtaque);
        } else {
            moverCriaturaAleatoriamente(*criatura);
        }
    }

    return mensajes;
}

std::optional<Jugador> Juego::buscarJugadorCercano(const Criatura& criatura) const {
    const Posicion posicionCriatura = criatura.getPos();

    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        const Posicion posicionJugador = jugador.getPosicion();

        if (!posicionCriatura.mismaMapa(posicionJugador)) {
            continue;
        }

        if (mapa.esZonaSegura(posicionJugador)) {
            continue;
        }

        if (posicionCriatura.distanciaManhattan(posicionJugador) <= criatura.getAggro()) {
            return jugador;
        }
    }

    return std::nullopt;
}

std::vector<Posicion> Juego::calcularDestinosAdyacentes(const Posicion& origen) const {
    std::vector<Posicion> destinos;

    if (origen.y > 0) {
        destinos.push_back(Posicion{origen.x, static_cast<uint16_t>(origen.y - 1), origen.mapaId});
    }

    destinos.push_back(Posicion{origen.x, static_cast<uint16_t>(origen.y + 1), origen.mapaId});

    if (origen.x > 0) {
        destinos.push_back(Posicion{static_cast<uint16_t>(origen.x - 1), origen.y, origen.mapaId});
    }

    destinos.push_back(Posicion{static_cast<uint16_t>(origen.x + 1), origen.y, origen.mapaId});

    return destinos;
}

std::vector<Posicion> Juego::calcularDestinosHacia(const Posicion& origen,
                                                   const Posicion& objetivo) const {
    std::vector<Posicion> destinos;

    if (objetivo.x > origen.x) {
        destinos.push_back(Posicion{static_cast<uint16_t>(origen.x + 1), origen.y, origen.mapaId});
    } else if (objetivo.x < origen.x && origen.x > 0) {
        destinos.push_back(Posicion{static_cast<uint16_t>(origen.x - 1), origen.y, origen.mapaId});
    }

    if (objetivo.y > origen.y) {
        destinos.push_back(Posicion{origen.x, static_cast<uint16_t>(origen.y + 1), origen.mapaId});
    } else if (objetivo.y < origen.y && origen.y > 0) {
        destinos.push_back(Posicion{origen.x, static_cast<uint16_t>(origen.y - 1), origen.mapaId});
    }

    return destinos;
}

void Juego::moverCriaturaAleatoriamente(const Criatura& criatura) {
    static std::random_device randomDevice;
    static std::mt19937 generador(randomDevice());

    std::vector<Posicion> destinosValidos;

    for (const Posicion& destino : calcularDestinosAdyacentes(criatura.getPos())) {
      if (puedeMoverCriaturaA(destino)) {
        destinosValidos.push_back(destino);
      }
    }

    if (destinosValidos.empty()) {
        return;
    }

    std::uniform_int_distribution<size_t> distribucion(0, destinosValidos.size() - 1);
    const Posicion destino = destinosValidos[distribucion(generador)];
    if (!mapa.moverCriatura(criatura.getId(), destino)) {
        return;
    }
}

std::list<EventoSalida> Juego::moverCriaturaHacia(const Criatura& criatura, const Posicion& objetivo) {
    const Posicion origen = criatura.getPos();

    if (origen.esAdyacente(objetivo)) {
        std::optional<uint16_t> idJugador = buscarIdJugadorEn(objetivo);

        if (idJugador.has_value()) {
            return atacarJugadorConCriatura(criatura, *idJugador);
        }

        return {};
    }

    for (const Posicion& destino : calcularDestinosHacia(origen, objetivo)) {
        if (puedeMoverCriaturaA(destino)) {
            if (mapa.moverCriatura(criatura.getId(), destino)) {
                return {};
            }
        }
    }

    moverCriaturaAleatoriamente(criatura);
    return {};
}

bool Juego::puedeMoverCriaturaA(const Posicion& destino) const {
  return mapa.puedeOcuparCriatura(destino) && !buscarIdJugadorEn(destino).has_value();
}

std::list<EventoSalida> Juego::atacarJugadorConCriatura(const Criatura& criatura, uint16_t idJugador) {
    std::list<EventoSalida> mensajes;

    Jugador* jugador = buscarJugador(idJugador);

    if (!jugador || !jugador->estaVivo()) {
        return mensajes;
    }

    if (mapa.esZonaSegura(jugador->getPosicion()) || mapa.esZonaSegura(criatura.getPos())) {
        return mensajes;
    }

    const uint16_t danio = jugador->recibir_ataque_fisico(criatura.calcularDanio(), catalogo);

    mensajes.push_back(EventoSalida{
        TipoDestino::UNO, idJugador,
        EventoDanioRecibido{ danio, criatura.getId() }
    });

    mensajes.push_back(armarEstado(idJugador, *jugador));

    if (!jugador->estaVivo()) {
        const EventoMuerteEntidad eventoMuerte{ jugador->getId() };
        const Posicion posicionJugador = jugador->getPosicion();

        for (const auto& [idCliente, otroJugador] : jugadoresConectados) {
            if (otroJugador.getPosicion().mapaId == posicionJugador.mapaId) {
                mensajes.push_back(EventoSalida{
                    TipoDestino::UNO, idCliente, eventoMuerte
                });
            }
        }

        const std::vector<uint16_t> itemsDropear = jugador->vaciar_inventario();

        for (uint16_t idItem : itemsDropear) {
            Posicion posicionDrop = posicionJugador;

            if (agregarItemEnSueloCercano(posicionJugador, idItem, posicionDrop)) {
                mensajes.splice(mensajes.end(), armarItemEnSueloParaMapa(posicionDrop, idItem));
            }
        }

        mensajes.push_back(armarInventario(idJugador, *jugador));

        std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(*jugador);
        mensajes.splice(mensajes.end(), mensajesPosicion);
    }

    return mensajes;
}

bool Juego::agregarCriatura(const Criatura& criatura) {
    const Posicion posicion = criatura.getPos();

    if (!mapa.puedeOcuparCriatura(posicion)) {
        return false;
    }

    if (buscarIdJugadorEn(posicion).has_value()) {
        return false;
    }

    return mapa.agregarCriatura(criatura);
}
