#include "juego.h"

#include <iostream>
#include <array>
#include <cstdlib>
#include <limits>
#include <list>
#include <optional>
#include <random>
#include <set>
#include <utility>
#include <variant>
#include <vector>
#include "../../common/protocolo/estado_entidad.h"
#include "../../common/protocolo/tipo_entidad.h"
#include "objeto/catalogo_items.h"
#include "reglas/reglas_juego.h"
#include "../persistencia/serializador_jugador.h"
#include "../../common/persistencia/error_persistencia.h"

Juego::Juego(const ConfigJuego& cfg, CatalogoItems&& cat, Mapa&& mapa) :
        cfg(cfg),
        catalogo(std::move(cat)),
        proximoIdClan(1),
        proximoIdCriatura(std::numeric_limits<uint16_t>::max()),
        mapa(std::move(mapa)),
        ticksTranscurridos(0),
        aleatorio(),
        indiceJugadores(cfg.rutaIndiceJugadores),
        lectorJugadores(cfg.rutaJugadores, indiceJugadores),
        escritorJugadores(cfg.rutaJugadores, indiceJugadores) {
    
    for (const EntradaStockComerciante& e : cfg.stockComerciante) {
        this->mapa.agregarStockComerciantes(e.id, e.precioCompra, e.precioVenta);
    }
    for (const EntradaStockSacerdote& e : cfg.stockSacerdote) {
        this->mapa.agregarStockSacerdotes(e.id, e.precio);
    }
}


std::list<EventoSalida> Juego::conectarJugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, uint16_t cabeza, uint16_t cuerpo) {

    if (jugadoresConectados.find(id) != jugadoresConectados.end()) {
        return {armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    auto itNickConectado = indiceNicksConectados.find(nombre);
    if (itNickConectado != indiceNicksConectados.end() && itNickConectado->second != id) {
        return {armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    auto itDesconectado = jugadoresDesconectados.end();

    for (auto it = jugadoresDesconectados.begin(); it != jugadoresDesconectados.end(); ++it) {
        if (it->second.getNombre() == nombre) {
            itDesconectado = it;
            break;
        }
    }

    // Si no es una reconexion de esta misma corrida (no esta en RAM), intentamos
    // cargar su progreso del disco. Los clanes no se persisten, asi que si el
    // registro traia un clan lo limpiamos para no dejar una referencia colgada.
    std::optional<Jugador> jugadorDisco;
    if (itDesconectado == jugadoresDesconectados.end()) {
        try {
            std::optional<RegistroJugador> registro = lectorJugadores.cargar(nombre);
            if (registro.has_value()) {
                jugadorDisco.emplace(SerializadorJugador::aJugador(id, *registro, cfg));
                if (jugadorDisco->tieneClan()) {
                    jugadorDisco->salirClan();
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[persistencia] error cargando " << nombre << ": " << e.what()
                      << std::endl;
        }
    }

    // Reconexion: la posicion deseada es la ultima conocida (RAM o disco). Conexion
    // nueva: el ancla del TOML. En todos los casos delegamos al BFS para garantizar
    // colision absoluta (regla 2.3) y evitar apilar avatares al spawn.
    const Posicion posicionDeseada =
            (itDesconectado != jugadoresDesconectados.end()) ? itDesconectado->second.getPosicion()
            : jugadorDisco.has_value()                       ? jugadorDisco->getPosicion()
                                                             : cfg.spawnInicial;

    const std::optional<Posicion> posicionResuelta = buscarPosicionLibreCercaDe(posicionDeseada);
    if (!posicionResuelta.has_value()) {
        return {armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (itDesconectado != jugadoresDesconectados.end()) {
        jugadoresConectados.emplace(id, std::move(itDesconectado->second));
        jugadoresDesconectados.erase(itDesconectado);
        jugadoresConectados.at(id).mover_a(posicionResuelta->x, posicionResuelta->y);
    } else if (jugadorDisco.has_value()) {
        if (existeIdPersonaje(id)) {
            return {armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
        }
        jugadoresConectados.emplace(id, std::move(*jugadorDisco));
        jugadoresConectados.at(id).mover_a(posicionResuelta->x, posicionResuelta->y);
    } else {
        if (existeIdPersonaje(id)) {
            return {armarError(id, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
        }
        jugadoresConectados.emplace(id, Jugador(id, nombre, clase, raza, cabeza, cuerpo, *posicionResuelta, cfg));
    }

    indiceNicksConectados[nombre] = id;

    Jugador& jugador = jugadoresConectados.at(id);
    std::list<EventoSalida> mensajes = {armarEstado(id, jugador), armarInventario(id, jugador),
                                        armarEquipamiento(id, jugador)};

    mensajes.splice(mensajes.end(), armarPosicionParaMapa(jugador));

    for (const auto& [idOtro, otro] : jugadoresConectados) {
        if (idOtro != id && otro.getPosicion().mapaId == jugador.getPosicion().mapaId) {
            mensajes.push_back(armarPosicionPara(id, otro));
        }
    }

    for (const Criatura& criatura : mapa.obtenerCriaturas()) {
        if (criatura.getPos().mapaId == jugador.getPosicion().mapaId) {
            mensajes.push_back(armarPosicionCriaturaPara(id, criatura));
        }
    }

    for (const ItemEnSuelo& item : mapa.obtenerItemsEnSuelo()) {
        if (item.posicion.mapaId == jugador.getPosicion().mapaId) {
            mensajes.push_back({TipoDestino::UNO, id,
                                EventoItemEnSuelo{item.idItem, item.posicion.x, item.posicion.y}});
        }
    }

    for (const OroEnSuelo& oro : mapa.obtenerOroEnSuelo()) {
        if (oro.posicion.mapaId == jugador.getPosicion().mapaId) {
            mensajes.push_back({TipoDestino::UNO, id,
                                EventoOroEnSuelo{oro.cantidad, oro.posicion.x, oro.posicion.y}});
        }
    }

    if (jugador.tieneClan()) {
        mensajes.splice(mensajes.end(), armarEventoClanParaMiembrosOnline(jugador.getClan(),
                                                                          TipoEventoClan::Conectado,
                                                                          jugador.getNombre(), id));
    }

    return mensajes;
}

std::list<EventoSalida> Juego::desconectarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    if (it == jugadoresConectados.end()) {
        return {};
    }

    std::list<EventoSalida> mensajes = armarDesaparicionParaMapa(it->second);

    if (it->second.tieneClan()) {
        mensajes.splice(mensajes.end(), armarEventoClanParaMiembrosOnline(
                                                it->second.getClan(), TipoEventoClan::Desconectado,
                                                it->second.getNombre(), id));
    }

    const std::string nombre = it->second.getNombre();
    indiceNicksConectados.erase(nombre);

    // Persistimos el progreso a disco (cross-restart) y ademas lo mantenemos en
    // RAM para reconexion rapida y operaciones de clan sobre miembros offline.
    guardarJugador(it->second);

    jugadoresDesconectados.emplace(id, std::move(it->second));
    jugadoresConectados.erase(it);

    return mensajes;
}

void Juego::guardarJugador(const Jugador& jugador) {
    try {
        const RegistroJugador registro = SerializadorJugador::aRegistro(jugador);
        escritorJugadores.guardar(registro);
    } catch (const std::exception& e) {
        std::cerr << "[persistencia] error guardando " << jugador.getNombre() << ": "
                  << e.what() << std::endl;
    }
}

void Juego::persistirConectados() {
    for (const auto& [id, jugador] : jugadoresConectados) {
        guardarJugador(jugador);
    }
}

void Juego::persistirTodos() {
    for (const auto& [id, jugador] : jugadoresConectados) {
        guardarJugador(jugador);
    }
    for (const auto& [id, jugador] : jugadoresDesconectados) {
        guardarJugador(jugador);
    }
}

Jugador* Juego::buscarJugador(uint16_t id) {
    auto it = jugadoresConectados.find(id);
    return (it != jugadoresConectados.end()) ? &it->second : nullptr;
}

Jugador* Juego::buscarJugadorPorIdPersonaje(uint16_t idPersonaje) {
    for (auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getId() == idPersonaje) {
            return &jugador;
        }
    }

    return nullptr;
}

std::optional<uint16_t> Juego::buscarIdClienteDeJugador(uint16_t idPersonaje) const {
    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getId() == idPersonaje) {
            return idCliente;
        }
    }

    return std::nullopt;
}

bool Juego::existeIdPersonaje(uint16_t idPersonaje) const {
    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getId() == idPersonaje) {
            return true;
        }
    }

    for (const auto& [idCliente, jugador] : jugadoresDesconectados) {
        if (jugador.getId() == idPersonaje) {
            return true;
        }
    }

    return false;
}

std::optional<uint16_t> Juego::buscarIdJugadorEn(const Posicion& posicion, std::optional<uint16_t> idExcluido) const {
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

std::optional<Posicion> Juego::buscarPosicionLibreCercaDe(
        const Posicion& origen, std::optional<uint16_t> idJugadorExcluido) const {
    if (!mapa.posicionValida(origen)) {
        return std::nullopt;
    }

    std::vector<Posicion> pendientes;
    std::set<std::pair<uint16_t, uint16_t>> visitadas;

    pendientes.push_back(origen);
    visitadas.insert({origen.x, origen.y});

    static constexpr std::array<std::pair<int, int>, 4> direcciones = {
            {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};

    for (std::vector<Posicion>::size_type indice = 0; indice < pendientes.size(); ++indice) {
        const Posicion actual = pendientes[indice];
        if (!mapa.hayParedEn(actual) && !mapa.hayNpcEn(actual) && !mapa.hayCriaturaEn(actual) &&
            !mapa.hayItemEn(actual) && !buscarIdJugadorEn(actual, idJugadorExcluido).has_value()) {
            return actual;
        }

        for (const auto& [dx, dy] : direcciones) {
            if ((dx < 0 && actual.x == 0) || (dy < 0 && actual.y == 0)) {
                continue;
            }

            const uint16_t nx = static_cast<uint16_t>(static_cast<int>(actual.x) + dx);
            const uint16_t ny = static_cast<uint16_t>(static_cast<int>(actual.y) + dy);
            const Posicion vecina{nx, ny, origen.mapaId};

            if (!mapa.posicionValida(vecina)) {
                continue;
            }

            if (visitadas.insert({nx, ny}).second) {
                pendientes.push_back(vecina);
            }
        }
    }

    return std::nullopt;
}

size_t Juego::contarAliadosClanCercanos(const Jugador& jugador) const {
    if (!jugador.tieneClan()) {
        return 0;
    }

    size_t aliados = 0;
    const Posicion posicionJugador = jugador.getPosicion();

    for (const auto& [idCliente, otro] : jugadoresConectados) {
        if (otro.getId() == jugador.getId() || !otro.estaVivo() || !otro.tieneClan() ||
            otro.getClan() != jugador.getClan()) {
            continue;
        }

        const Posicion posicionOtro = otro.getPosicion();
        if (posicionJugador.mismaMapa(posicionOtro) &&
            posicionJugador.distanciaManhattan(posicionOtro) <= cfg.clanRadioBonus) {
            aliados++;
        }
    }

    return aliados;
}

float Juego::multiplicadorClan(const Jugador& jugador) const {
    return ReglasJuego::calcularMultiplicadorClan(cfg, contarAliadosClanCercanos(jugador));
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
        if (c.getNombre() == nombre)
            return &c;
    return nullptr;
}

EventoSalida Juego::armarError(uint16_t idCliente, CodigoErrorAccion cod) {
    return {TipoDestino::UNO, idCliente, EventoErrorAccion{cod}};
}

EventoSalida Juego::armarEstado(uint16_t idCliente, const Jugador& jugador) {
    return {TipoDestino::UNO, idCliente,
            EventoEstadoPersonaje{jugador.getVidaActual(), jugador.getVidaMax(),
                                  jugador.getManaActual(), jugador.getManaMax(), jugador.getOro(),
                                  jugador.getNivel(), jugador.getExperiencia(),
                                  jugador.getEstado()}};
}

EventoSalida Juego::armarInventario(uint16_t idCliente, const Jugador& jugador) {
    return {TipoDestino::UNO, idCliente, EventoActualizarInventario{jugador.getSlotsInventario()}};
}

EventoSalida Juego::armarEquipamiento(uint16_t idCliente, const Jugador& jugador) {
    return {TipoDestino::UNO, idCliente,
            EventoActualizarEquipamiento{jugador.getArmaEquipada(), jugador.getBaculoEquipado(),
                                         jugador.getDefensaEquipada(), jugador.getCascoEquipado(),
                                         jugador.getEscudoEquipado()}};
}

EventoSalida Juego::armarPosicionPara(uint16_t idCliente, const Jugador& jugador) {
    Posicion posicion = jugador.getPosicion();
    return {TipoDestino::UNO, idCliente,
            EventoPosicionEntidad{jugador.getId(), posicion.x, posicion.y,
                                  static_cast<uint8_t>(TipoEntidad::Personaje),
                                  estadoEntidadDe(jugador),
                                  jugador.getCabeza(), 
                                  jugador.getCuerpo()}};
}

EventoSalida Juego::armarPosicionCriaturaPara(uint16_t idCliente, const Criatura& criatura) {
    Posicion posicion = criatura.getPos();
    return {TipoDestino::UNO, idCliente,
            EventoPosicionEntidad{criatura.getId(), posicion.x, posicion.y,
                                  static_cast<uint8_t>(TipoEntidad::Criatura),
                                  static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo),
                                  0, // Cabeza de criatura es 0
                                  criatura.getCuerpo()}};
}

uint8_t Juego::estadoEntidadDe(const Jugador& jugador) const {
    switch (jugador.getEstado()) {
        case Estado::Vivo:
            return static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo);
        case Estado::Fantasma:
            return static_cast<uint8_t>(EstadoEntidadProtocolo::Fantasma);
        case Estado::Meditando:
            return static_cast<uint8_t>(EstadoEntidadProtocolo::Meditando);
        case Estado::Resucitando:
            return static_cast<uint8_t>(EstadoEntidadProtocolo::Resucitando);
    }

    return static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo);
}

std::list<EventoSalida> Juego::armarPosicionParaMapa(const Jugador& jugador) {
    std::list<EventoSalida> mensajes;
    Posicion posicion = jugador.getPosicion();

    for (const auto& [idCliente, otro] : jugadoresConectados) {
        if (otro.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back(armarPosicionPara(idCliente, jugador));
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarPosicionCriaturaParaMapa(const Criatura& criatura) {
    std::list<EventoSalida> mensajes;
    Posicion posicion = criatura.getPos();

    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back(armarPosicionCriaturaPara(idCliente, criatura));
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarDesaparicionParaMapa(const Jugador& jugador) {
    std::list<EventoSalida> mensajes;
    Posicion posicion = jugador.getPosicion();

    for (const auto& [idCliente, otro] : jugadoresConectados) {
        if (otro.getId() != jugador.getId() && otro.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back(
                    {TipoDestino::UNO, idCliente, EventoEntidadDesaparecio{jugador.getId()}});
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarItemEnSueloParaMapa(const Posicion& posicion, uint16_t idItem) {
    std::list<EventoSalida> mensajes;

    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back({TipoDestino::UNO, idCliente,
                                EventoItemEnSuelo{idItem, posicion.x, posicion.y}});
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarItemDesaparecioSueloParaMapa(const Posicion& posicion) {
    std::list<EventoSalida> mensajes;

    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back({TipoDestino::UNO, idCliente,
                                EventoItemDesaparecioSuelo{posicion.x, posicion.y}});
        }
    }

    return mensajes;
}

std::list<EventoSalida> Juego::armarEventoClanParaMiembrosOnline( uint16_t idClan, TipoEventoClan tipo, const std::string& texto, std::optional<uint16_t> idExcluido) const {
    std::list<EventoSalida> mensajes;

    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (idExcluido.has_value() && idCliente == *idExcluido) {
            continue;
        }

        if (jugador.tieneClan() && jugador.getClan() == idClan) {
            mensajes.push_back({TipoDestino::UNO, idCliente, EventoClan{tipo, texto}});
        }
    }

    return mensajes;
}

bool Juego::agregarItemEnSueloCercano(const Posicion& origen, uint16_t idItem,
                                      Posicion& posicionFinal) {
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

    auto finalizar = [&](std::list<EventoSalida> mensajes,
                         bool emitirPosicionSiEsMover = false) {
        if (canceloMeditacion && (emitirPosicionSiEsMover || comando.opcode != Opcode::MOVER)) {
            if (Jugador* jugador = buscarJugador(idCliente)) {
                std::list<EventoSalida> mensajesPosicion = armarPosicionParaMapa(*jugador);
                mensajes.splice(mensajes.end(), mensajesPosicion);
            }
        }
        return mensajes;
    };

    auto comandoInvalido = [&]() {
        return finalizar({armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)}, true);
    };

    auto ejecutarSinPayload = [&](bool payloadValido, auto ejecutar) -> std::list<EventoSalida> {
        if (!payloadValido) {
            return comandoInvalido();
        }
        return finalizar(ejecutar());
    };

    auto ejecutarConPayload = [&](const auto* payload, auto ejecutar) -> std::list<EventoSalida> {
        if (payload == nullptr) {
            return comandoInvalido();
        }
        return finalizar(ejecutar(*payload));
    };

    switch (comando.opcode) {
        case Opcode::MEDITAR:
            return ejecutarSinPayload(std::holds_alternative<ComandoMeditar>(comando.payload),
                                      [&]() { return ejecutarMeditar(idCliente); });
        case Opcode::RESUCITAR:
            return ejecutarSinPayload(std::holds_alternative<ComandoResucitar>(comando.payload),
                                      [&]() { return ejecutarResucitar(idCliente); });
        case Opcode::TOMAR:
            return ejecutarSinPayload(std::holds_alternative<ComandoTomar>(comando.payload),
                                      [&]() { return ejecutarTomar(idCliente); });
        case Opcode::REVISAR_CLAN:
            return ejecutarSinPayload(std::holds_alternative<ComandoRevisarClan>(comando.payload),
                                      [&]() { return ejecutarRevisarClan(idCliente); });
        case Opcode::DEJAR_CLAN:
            return ejecutarSinPayload(std::holds_alternative<ComandoDejarClan>(comando.payload),
                                      [&]() { return ejecutarDejarClan(idCliente); });
        case Opcode::MOVER:
            return ejecutarConPayload(std::get_if<ComandoMover>(&comando.payload),
                                      [&](const ComandoMover& payload) {
                                          return ejecutarMover(idCliente, payload);
                                      });
        case Opcode::ATACAR:
            return ejecutarConPayload(std::get_if<ComandoAtacar>(&comando.payload),
                                      [&](const ComandoAtacar& payload) {
                                          return ejecutarAtacar(idCliente, payload);
                                      });
        case Opcode::TIRAR:
            return ejecutarConPayload(std::get_if<ComandoTirar>(&comando.payload),
                                      [&](const ComandoTirar& payload) {
                                          return ejecutarTirar(idCliente, payload);
                                      });
        case Opcode::EQUIPAR:
            return ejecutarConPayload(std::get_if<ComandoEquipar>(&comando.payload),
                                      [&](const ComandoEquipar& payload) {
                                          return ejecutarEquipar(idCliente, payload);
                                      });
        case Opcode::COMPRAR:
            return ejecutarConPayload(std::get_if<ComandoComprar>(&comando.payload),
                                      [&](const ComandoComprar& payload) {
                                          return ejecutarComprar(idCliente, payload);
                                      });
        case Opcode::VENDER:
            return ejecutarConPayload(std::get_if<ComandoVender>(&comando.payload),
                                      [&](const ComandoVender& payload) {
                                          return ejecutarVender(idCliente, payload);
                                      });
        case Opcode::DEPOSITAR_ITEM:
            return ejecutarConPayload(std::get_if<ComandoDepositarItem>(&comando.payload),
                                      [&](const ComandoDepositarItem& payload) {
                                          return ejecutarDepositarItem(idCliente, payload);
                                      });
        case Opcode::DEPOSITAR_ORO:
            return ejecutarConPayload(std::get_if<ComandoDepositarOro>(&comando.payload),
                                      [&](const ComandoDepositarOro& payload) {
                                          return ejecutarDepositarOro(idCliente, payload);
                                      });
        case Opcode::RETIRAR_ITEM:
            return ejecutarConPayload(std::get_if<ComandoRetirarItem>(&comando.payload),
                                      [&](const ComandoRetirarItem& payload) {
                                          return ejecutarRetirarItem(idCliente, payload);
                                      });
        case Opcode::RETIRAR_ORO:
            return ejecutarConPayload(std::get_if<ComandoRetirarOro>(&comando.payload),
                                      [&](const ComandoRetirarOro& payload) {
                                          return ejecutarRetirarOro(idCliente, payload);
                                      });
        case Opcode::LISTAR:
            return ejecutarConPayload(std::get_if<ComandoListar>(&comando.payload),
                                      [&](const ComandoListar& payload) {
                                          return ejecutarListar(idCliente, payload);
                                      });
        case Opcode::CURAR:
            return ejecutarConPayload(std::get_if<ComandoCurar>(&comando.payload),
                                      [&](const ComandoCurar& payload) {
                                          return ejecutarCurar(idCliente, payload);
                                      });
        case Opcode::CHAT_GLOBAL:
            return ejecutarConPayload(std::get_if<ComandoChatGlobal>(&comando.payload),
                                      [&](const ComandoChatGlobal& payload) {
                                          return ejecutarChatGlobal(idCliente, payload);
                                      });
        case Opcode::CHAT_PRIVADO:
            return ejecutarConPayload(std::get_if<ComandoChatPrivado>(&comando.payload),
                                      [&](const ComandoChatPrivado& payload) {
                                          return ejecutarChatPrivado(idCliente, payload);
                                      });
        case Opcode::FUNDAR_CLAN:
            return ejecutarConPayload(std::get_if<ComandoFundarClan>(&comando.payload),
                                      [&](const ComandoFundarClan& payload) {
                                          return ejecutarFundarClan(idCliente, payload);
                                      });
        case Opcode::UNIRSE_CLAN:
            return ejecutarConPayload(std::get_if<ComandoUnirseClan>(&comando.payload),
                                      [&](const ComandoUnirseClan& payload) {
                                          return ejecutarUnirseClan(idCliente, payload);
                                      });
        case Opcode::CLAN_ACEPTAR:
        case Opcode::CLAN_RECHAZAR:
        case Opcode::CLAN_BAN:
        case Opcode::CLAN_KICK:
            return ejecutarConPayload(std::get_if<ComandoGestionMiembreClan>(&comando.payload),
                                      [&](const ComandoGestionMiembreClan& payload) {
                                          return ejecutarGestionMiembroClan(
                                                  idCliente, payload, comando.opcode);
                                      });
        case Opcode::CHEAT:
            return ejecutarConPayload(std::get_if<ComandoCheat>(&comando.payload),
                                      [&](const ComandoCheat& payload) {
                                          return ejecutarCheat(idCliente, payload);
                                      });
        default:
            return comandoInvalido();
    }
}

// ─── Tick del mundo ───────────────────────────────────────────────────────────
std::list<EventoSalida> Juego::actualizar(float deltaSegundos) {
    std::list<EventoSalida> mensajes;
    ticksTranscurridos++;

    for (auto& [id, jugador] : jugadoresConectados) {
        const bool estabaVivo = jugador.estaVivo();
        const bool estabaMeditando = jugador.enMeditacion();
        const bool estabaInmovilizado = jugador.estaInmovilizado();
        const Posicion posicionAntes = jugador.getPosicion();

        const uint16_t vidaAntes = jugador.getVidaActual();
        const uint16_t manaAntes = jugador.getManaActual();
        const uint32_t oroAntes = jugador.getOro();
        const uint16_t nivelAntes = jugador.getNivel();
        const uint32_t experienciaAntes = jugador.getExperiencia();
        const Estado estadoAntes = jugador.getEstado();

        jugador.recuperar(deltaSegundos);

        const bool cambioEstado =
                vidaAntes != jugador.getVidaActual() || manaAntes != jugador.getManaActual() ||
                oroAntes != jugador.getOro() || nivelAntes != jugador.getNivel() ||
                experienciaAntes != jugador.getExperiencia() || estadoAntes != jugador.getEstado();

        if (cambioEstado) {
            mensajes.push_back(armarEstado(id, jugador));
        }
        // Dispatch único de transiciones de estado del jugador: muerte, fin de inmovilización y fin de meditación son mutuamente excluyentes.
        if (estabaVivo && !jugador.estaVivo()) {
            mensajes.splice(mensajes.end(), emitirMuerteJugador(jugador, posicionAntes));
        } else if (estabaInmovilizado && !jugador.estaInmovilizado()) {
            Posicion posicionResurreccion = jugador.getPosicionResurreccion();
            std::optional<Posicion> posicionResurreccionCercana =
                    buscarPosicionLibreCercaDe(posicionResurreccion, id);
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

    if (cfg.movimientoCriaturasTicks > 0 &&
        ticksTranscurridos % cfg.movimientoCriaturasTicks == 0) {
        std::list<EventoSalida> mensajesCriaturas = actualizarCriaturas();
        mensajes.splice(mensajes.end(), mensajesCriaturas);
    }

    if (cfg.spawnCriaturasTicks > 0 && ticksTranscurridos % cfg.spawnCriaturasTicks == 0 &&
        mapa.cantidadCriaturas() < cfg.poblacionMaxCriaturas) {
        std::list<EventoSalida> mensajesSpawn = intentarSpawnCriatura();
        mensajes.splice(mensajes.end(), mensajesSpawn);
    }

    std::vector<ItemEnSuelo> itemsExpirados =
            mapa.actualizarItemsEnSuelo(deltaSegundos, cfg.tiempoItemSueloSeg);

    for (const ItemEnSuelo& item : itemsExpirados) {
        std::list<EventoSalida> mensajesItem = armarItemDesaparecioSueloParaMapa(item.posicion);
        mensajes.splice(mensajes.end(), mensajesItem);
    }

    std::vector<OroEnSuelo> orosExpirados =
            mapa.actualizarOroEnSuelo(deltaSegundos, cfg.tiempoItemSueloSeg);

    for (const OroEnSuelo& pila : orosExpirados) {
        std::list<EventoSalida> mensajesOro = armarOroDesaparecioSueloParaMapa(pila.posicion);
        mensajes.splice(mensajes.end(), mensajesOro);
    }

    return mensajes;
}

// Meditar 

std::list<EventoSalida> Juego::ejecutarMeditar(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador)
        return {};

    if (!jugador->estaVivo() || !jugador->puedeMeditar())
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};

    jugador->meditar();

    std::list<EventoSalida> mensajes = {armarEstado(idCliente, *jugador)};

    mensajes.splice(mensajes.end(), armarPosicionParaMapa(*jugador));
    return mensajes;
}

// ─── Cheats de prueba
std::list<EventoSalida> Juego::ejecutarCheat(uint16_t idCliente, const ComandoCheat& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador) {
        return {};
    }

    switch (static_cast<TipoCheat>(comando.tipo)) {
        case TipoCheat::VidaInfinita:
            jugador->alternarVidaInfinita();
            return {armarEstado(idCliente, *jugador)};

        case TipoCheat::ManaInfinito:
            jugador->alternarManaInfinito();
            return {armarEstado(idCliente, *jugador)};

        case TipoCheat::MorirAuto: {
            if (!jugador->estaVivo()) {
                return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
            }
            const Posicion posicionMuerte = jugador->getPosicion();
            jugador->matar();
            std::list<EventoSalida> mensajes = {armarEstado(idCliente, *jugador)};
            mensajes.splice(mensajes.end(), emitirMuerteJugador(*jugador, posicionMuerte));
            return mensajes;
        }

        default:
            return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }
}

// ─── Chat
std::list<EventoSalida> Juego::ejecutarChatGlobal(uint16_t idCliente,
                                                  const ComandoChatGlobal& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    return {{TipoDestino::TODOS, 0, EventoChat{jugador->getNombre(), comando.mensaje}}};
}

std::list<EventoSalida> Juego::ejecutarChatPrivado(uint16_t idCliente,
                                                   const ComandoChatPrivado& comando) {
    Jugador* emisor = buscarJugador(idCliente);
    if (!emisor || !emisor->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    auto itDestino = indiceNicksConectados.find(comando.nickDestino);
    if (itDestino == indiceNicksConectados.end()) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    return {{TipoDestino::UNO, itDestino->second,
             EventoChat{emisor->getNombre(), comando.mensaje}}};
}

// ─── Clan 

std::list<EventoSalida> Juego::ejecutarFundarClan(uint16_t idCliente,
                                                  const ComandoFundarClan& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador)
        return {};

    if (!jugador->estaVivo() || jugador->tieneClan()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (jugador->getNivel() < cfg.clanNivelMinimo) {
        return {armarError(idCliente, CodigoErrorAccion::NIVEL_INSUFICIENTE)};
    }

    if (buscarClanPorNombre(comando.nombreClan)) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }


    uint16_t idClan = proximoIdClan++;
    clanes.emplace(idClan, Clan(idClan, comando.nombreClan, jugador->getNombre()));
    jugador->asignarClan(idClan);
    jugador->marcarFundadorClan();

    return {{TipoDestino::UNO, idCliente, EventoClan{TipoEventoClan::Fundado, comando.nombreClan}}};
}

std::list<EventoSalida> Juego::ejecutarUnirseClan(uint16_t idCliente,
                                                  const ComandoUnirseClan& comando) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador)
        return {};

    if (!jugador->estaVivo() || jugador->tieneClan()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Clan* clan = buscarClanPorNombre(comando.nombreClan);
    if (!clan) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    const std::string nickSolicitante = jugador->getNombre();

    if (clan->estaBaneado(nickSolicitante))
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};


    if ((int)(clan->cantidadMiembros()) >= cfg.clanMaxMiembros) {
        return {armarError(idCliente, CodigoErrorAccion::CLAN_LLENO)};
    }


    clan->pedirUnirse(nickSolicitante);

    // Notificar al fundador si está conectado
    std::list<EventoSalida> mensajes;
    uint16_t idClan = clan->getId();
    for (auto& [id, jugador_] : jugadoresConectados) {
        if (jugador_.fundo_clan() && jugador_.getClan() == idClan) {
            mensajes.push_back({TipoDestino::UNO, id,
                                EventoClan{TipoEventoClan::MiembroPendiente, nickSolicitante}});
        }
    }
    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarDejarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo() || !jugador->tieneClan())
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    if (jugador->fundo_clan())
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};

    const uint16_t idClan = jugador->getClan();
    const std::string nombreJugador = jugador->getNombre();

    clanes.at(idClan).eliminarMiembro(nombreJugador);
    jugador->salirClan();

    return armarEventoClanParaMiembrosOnline(
            idClan, TipoEventoClan::Abandono, nombreJugador, idCliente);
}

std::list<EventoSalida> Juego::ejecutarRevisarClan(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo() || !jugador->tieneClan() || !jugador->fundo_clan())
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};

    Clan& clan = clanes.at(jugador->getClan());
    std::list<EventoSalida> mensajes;

    for (const std::string& nickPendiente : clan.obtenerPendientes()) {
        mensajes.push_back({TipoDestino::UNO, idCliente,
                            EventoClan{TipoEventoClan::MiembroPendiente, nickPendiente}});
    }

    for (const std::string& nickMiembro : clan.obtenerMiembros()) {
        mensajes.push_back({TipoDestino::UNO, idCliente,
                            EventoClan{TipoEventoClan::MiembroActivo, nickMiembro}});
    }

    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarGestionMiembroClan(uint16_t idCliente,
                                                          const ComandoGestionMiembreClan& comando,
                                                          Opcode accion) {

    Jugador* lider = buscarJugador(idCliente);
    if (!lider || !lider->estaVivo() || !lider->fundo_clan())
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};

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
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};

    if (idObjetivo == idCliente)
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};

    if ((accion == Opcode::CLAN_KICK || accion == Opcode::CLAN_BAN) &&
        clan.esFundador(comando.nick)) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    std::list<EventoSalida> msgs;

    switch (accion) {
        case Opcode::CLAN_ACEPTAR: {
            if (!clan.estaPendiente(comando.nick)) {
                return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
            }

            clan.agregarMiembro(comando.nick);
            // Actualizar el jugador conectado o desconectado
            if (objetivo) {
                objetivo->asignarClan(lider->getClan());
                msgs.push_back({TipoDestino::UNO, idObjetivo,
                                EventoClan{TipoEventoClan::Aceptado, clan.getNombre()}});
            } else {
                auto itDesc = jugadoresDesconectados.find(idObjetivo);
                if (itDesc != jugadoresDesconectados.end())
                    itDesc->second.asignarClan(lider->getClan());
            }
            break;
        }

        case Opcode::CLAN_RECHAZAR:
            if (!clan.estaPendiente(comando.nick)) {
                return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
            }

            clan.eliminarMiembro(comando.nick);
            if (objetivo)
                msgs.push_back({TipoDestino::UNO, idObjetivo,
                                EventoClan{TipoEventoClan::Rechazado, clan.getNombre()}});
            break;

        case Opcode::CLAN_BAN:
            if (!clan.esMiembro(comando.nick) && !clan.estaPendiente(comando.nick)) {
                return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
            }

            clan.banearMiembro(comando.nick);
            if (objetivo) {
                objetivo->salirClan();
                msgs.push_back({TipoDestino::UNO, idObjetivo,
                                EventoClan{TipoEventoClan::Baneado, clan.getNombre()}});
            } else {
                auto itDesc = jugadoresDesconectados.find(idObjetivo);
                if (itDesc != jugadoresDesconectados.end())
                    itDesc->second.salirClan();
            }
            break;

        case Opcode::CLAN_KICK:
            if (!clan.esMiembro(comando.nick)) {
                return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
            }

            clan.eliminarMiembro(comando.nick);
            if (objetivo) {
                objetivo->salirClan();
                msgs.push_back({TipoDestino::UNO, idObjetivo,
                                EventoClan{TipoEventoClan::Kickeado, clan.getNombre()}});
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

// ─── Mapa/Mundo 

std::list<EventoSalida> Juego::ejecutarResucitar(uint16_t idCliente) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->esFantasma() || jugador->estaInmovilizado()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    const Posicion posicionJugador = jugador->getPosicion();

    // En ciudad la resurrección es inmediata. Fuera de ciudad se aplica la mecánica remota: espera proporcional a la distancia al sacerdote.
    if (mapa.esCiudad(posicionJugador)) {
        std::optional<Posicion> posicionResurreccion =
                buscarPosicionLibreCercaDe(posicionJugador, idCliente);
        if (!posicionResurreccion.has_value()) {
            return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
        }

        jugador->resucitar(posicionResurreccion->x, posicionResurreccion->y);

        std::list<EventoSalida> mensajes = {armarEstado(idCliente, *jugador)};

        mensajes.splice(mensajes.end(), armarPosicionParaMapa(*jugador));
        return mensajes;
    }

    // Fuera de ciudad: buscar al sacerdote más cercano en el mapa del jugador.
    std::optional<Npc> npcSacerdote = mapa.buscarSacerdoteMasCercano(posicionJugador);
    if (!npcSacerdote.has_value()) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Valida via el accessor con puntero observador que el sacerdote efectivamente exista como entidad concreta en el mapa.
    const Sacerdote* sacerdote = mapa.obtenerSacerdote(npcSacerdote->getId());
    if (sacerdote == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    std::optional<Posicion> posicionResurreccion =
            buscarPosicionLibreCercaDe(sacerdote->getPosicion(), idCliente);
    if (!posicionResurreccion.has_value()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    const int distancia = posicionJugador.distanciaManhattan(sacerdote->getPosicion());
    const float tiempoInmovilizado = distancia * cfg.factorTiempoResurreccion;

    if (tiempoInmovilizado <= 0.0f) {
        jugador->resucitar(posicionResurreccion->x, posicionResurreccion->y);

        std::list<EventoSalida> mensajes = {
                armarEstado(idCliente, *jugador),
        };

        mensajes.splice(mensajes.end(), armarPosicionParaMapa(*jugador));
        return mensajes;
    }

    jugador->inmovilizar(posicionResurreccion->x, posicionResurreccion->y, tiempoInmovilizado);

    std::list<EventoSalida> mensajes = {
            armarEstado(idCliente, *jugador),
    };

    mensajes.splice(mensajes.end(), armarPosicionParaMapa(*jugador));
    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarTomar(uint16_t idCliente) {

    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Posicion posicion = jugador->getPosicion();

    // Prioridad: si hay una pila de oro en la celda, se levanta antes que los ítems.
    if (std::optional<uint32_t> cantidadOro = mapa.tomarOro(posicion); cantidadOro.has_value()) {
        jugador->sumar_oro(*cantidadOro);
        std::list<EventoSalida> mensajes = {armarEstado(idCliente, *jugador)};
        mensajes.splice(mensajes.end(), armarOroDesaparecioSueloParaMapa(posicion));
        return mensajes;
    }

    std::optional<uint16_t> idItem = mapa.tomarItem(posicion);

    if (!idItem.has_value()) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!catalogo.existe(*idItem)) {
        mapa.agregarItem(posicion, *idItem);
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!jugador->agregar_item(*idItem)) {
        mapa.agregarItem(posicion, *idItem);
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    std::list<EventoSalida> mensajes = {armarInventario(idCliente, *jugador)};

    mensajes.splice(mensajes.end(), armarItemDesaparecioSueloParaMapa(posicion));

    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarMover(uint16_t idCliente, const ComandoMover& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || (!jugador->estaVivo() && !jugador->esFantasma()) ||
        jugador->estaInmovilizado()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Posicion destino = jugador->getPosicion();

    switch (cmd.direccion) {
        case 0:
            if (destino.y == 0) {
                return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
            }
            destino.y--;
            break;

        case 1:
            destino.y++;
            break;

        case 2:
            if (destino.x == 0) {
                return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
            }
            destino.x--;
            break;

        case 3:
            destino.x++;
            break;

        default:
            return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Colisión absoluta: una celda no puede ser compartida por dos entidades.
    // Aplica tanto a jugadores vivos como a fantasmas — el enunciado no diferencia el modelo de colisión por estado del personaje.
    if (!mapa.posicionValida(destino) || mapa.hayParedEn(destino) || mapa.hayNpcEn(destino) ||
        mapa.hayCriaturaEn(destino)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (buscarIdJugadorEn(destino, idCliente).has_value()) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    jugador->mover_a(destino.x, destino.y);

    return armarPosicionParaMapa(*jugador);
}

std::list<EventoSalida> Juego::ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& cmd) {
    Jugador* atacante = buscarJugador(idCliente);

    if (!atacante || !atacante->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (cmd.idObjetivo == atacante->getId()) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Cooldown de ataque
    if (!atacante->puedeAtacar()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    // Resolución de objetivo: primero jugador (PvP), luego criatura (PvE).
    // El cooldown se consume sólo cuando hay una entidad real a la que golpear, para que el spam contra blancos inexistentes no quede limitado ni cause daño.
    if (buscarJugadorPorIdPersonaje(cmd.idObjetivo) != nullptr) {
        atacante->registrarAtaque();
        return ejecutarAtaqueAJugador(idCliente, *atacante, cmd);
    }

    if (Criatura* criaturaObjetivo = mapa.obtenerCriaturaPor(cmd.idObjetivo)) {
        atacante->registrarAtaque();
        return ejecutarAtaqueACriatura(idCliente, *atacante, *criaturaObjetivo);
    }

    return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
}

std::list<EventoSalida> Juego::ejecutarAtaqueAJugador(uint16_t idCliente, Jugador& atacanteRef,
                                                      const ComandoAtacar& cmd) {
    Jugador* atacante = &atacanteRef;
    Jugador* objetivo = buscarJugadorPorIdPersonaje(cmd.idObjetivo);
    const std::optional<uint16_t> idClienteObjetivo =
            buscarIdClienteDeJugador(cmd.idObjetivo);
    std::cout << "El jugador con id " << atacante->getId() << " ataca al jugador con id " << cmd.idObjetivo << std::endl;
    if (!objetivo || !idClienteObjetivo.has_value() || !objetivo->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    const Posicion posicionAtacante = atacante->getPosicion();
    const Posicion posicionObjetivo = objetivo->getPosicion();

    if (!posicionAtacante.mismaMapa(posicionObjetivo)) {
        std::cout << "Atacante y objetivo no están en el mismo mapa" << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (mapa.esZonaSegura(posicionAtacante) || mapa.esZonaSegura(posicionObjetivo)) {
        std::cout << "Atacante o objetivo están en zona segura" << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (atacante->es_newbie() || objetivo->es_newbie()) {
        std::cout << "Atacante o objetivo son nuevos" << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    const int diferenciaNivel = std::abs(static_cast<int>(atacante->getNivel()) -
                                         static_cast<int>(objetivo->getNivel()));

    if (diferenciaNivel > cfg.maxDiffNivel) {
        std::cout << "La diferencia de nivel entre atacante y objetivo es demasiado grande: " << diferenciaNivel << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (atacante->tieneClan() && objetivo->tieneClan() &&
        atacante->getClan() == objetivo->getClan()) {
        std::cout << "Atacante y objetivo pertenecen al mismo clan" << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    // El alcance depende del equipamiento del atacante.
    // Si es hechizo, el atacante debe tener maná suficiente.
    const DescriptorAtaque descriptorAtaque = atacante->describir_ataque(catalogo);

    if (descriptorAtaque.tipo == TipoAtaque::HechizoNoOfensivo) {
        std::cout << "El atacante intenta usar un hechizo no ofensivo" << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    const int distanciaAlObjetivo = posicionAtacante.distanciaManhattan(posicionObjetivo);
    if (distanciaAlObjetivo > descriptorAtaque.alcanceMaximo) {
        std::cout << "El objetivo está fuera del alcance del ataque: distancia " << distanciaAlObjetivo
                  << ", alcance máximo " << descriptorAtaque.alcanceMaximo << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    bool atacanteConsumioMana = false;
    if (descriptorAtaque.costoMana > 0) {
        if (!atacante->consumir_mana(descriptorAtaque.costoMana)) {
            std::cout << "El atacante no tiene mana suficiente" << std::endl;
            return {armarError(idCliente, CodigoErrorAccion::MANA_INSUFICIENTE)};
        }
        atacanteConsumioMana = true;
    }

    // Capturamos atributos del objetivo ANTES del golpe: si muere, su nivel y vidaMax podrían reiniciarse en la lógica de muerte/respawn.
    const uint8_t nivelAtacante = atacante->getNivel();
    const uint8_t nivelObjetivoAntes = objetivo->getNivel();
    const uint16_t vidaMaxObjetivoAntes = objetivo->getVidaMax();

    const ResultadoDanio resultadoDanio = atacante->calcular_danio(catalogo, aleatorio);
    const uint16_t danioBruto = ReglasJuego::aplicarMultiplicadorCombate(
            resultadoDanio.valor, multiplicadorClan(*atacante));
    const ResultadoDefensa resultadoDefensa = objetivo->recibir_ataque_fisico(
            danioBruto, resultadoDanio.esCritico, catalogo, aleatorio,
            multiplicadorClan(*objetivo));

    std::list<EventoSalida> mensajes;
    bool atacanteGanoXp = false;

    if (resultadoDefensa.tipo == ResultadoDefensa::Tipo::Esquivo) {
        // Simetría con PvE (ver ejecutarAtaqueACriatura): notificar al atacante y al defensor que la evasión ocurrió. Sin daño no hay XP de impacto,
        // BajoAtaque ni estado del defensor que reportar.
        mensajes.push_back(EventoSalida{TipoDestino::UNO, idCliente,
                                        EventoEsquive{objetivo->getId(), /*esquivador=*/1}});
        mensajes.push_back(EventoSalida{TipoDestino::UNO, *idClienteObjetivo,
                                        EventoEsquive{objetivo->getId(), /*esquivador=*/1}});
    } else {
        const uint16_t danioAplicado = resultadoDefensa.danioAplicado;
        std::cout << "El atacante con id " << atacante->getId() << " ataca al jugador con id " << objetivo->getId() << " y causa " << danioAplicado << " de daño" << std::endl;

        mensajes.push_back(EventoSalida{TipoDestino::UNO, idCliente,
                                        EventoDanioProducido{danioAplicado, objetivo->getId()}});

        mensajes.push_back(EventoSalida{TipoDestino::UNO, *idClienteObjetivo,
                                        EventoDanioRecibido{danioAplicado, atacante->getId()}});

        mensajes.push_back(armarEstado(*idClienteObjetivo, *objetivo));

        if (danioAplicado > 0 && objetivo->tieneClan()) {
            mensajes.splice(mensajes.end(), armarEventoClanParaMiembrosOnline(
                                                    objetivo->getClan(), TipoEventoClan::BajoAtaque,
                                                    objetivo->getNombre(), *idClienteObjetivo));
        }

        // XP por impacto. Sólo si el golpe entró con daño efectivo.
        if (danioAplicado > 0) {
            const uint32_t xpHit = ReglasJuego::calcularExperienciaImpacto(
                    cfg, danioAplicado, nivelAtacante, nivelObjetivoAntes);
            if (xpHit > 0) {
                atacante->ganar_experiencia(xpHit);
                atacanteGanoXp = true;
            }
        }
    }

    if (resultadoDefensa.tipo == ResultadoDefensa::Tipo::Golpeado && !objetivo->estaVivo()) {
        const float valorAleatorio = aleatorio.uniforme();

        const uint32_t xpKill = ReglasJuego::calcularExperienciaKill(
                cfg, vidaMaxObjetivoAntes, nivelAtacante, nivelObjetivoAntes, valorAleatorio);

        if (xpKill > 0) {
            atacante->ganar_experiencia(xpKill);
            atacanteGanoXp = true;
        }

        std::cout << "El jugador con id " << objetivo->getId() << " ha muerto" << std::endl;
        mensajes.splice(mensajes.end(), emitirMuerteJugador(*objetivo, objetivo->getPosicion()));
    }

    // Emitimos estado del atacante si cambió algo visible: XP/nivel ganado o maná consumido por hechizo.
    if (atacanteGanoXp || atacanteConsumioMana) {
        mensajes.push_back(armarEstado(idCliente, *atacante));
    }
    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarTirar(uint16_t idCliente, const ComandoTirar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Posicion posicion = jugador->getPosicion();

    if (mapa.hayItemEn(posicion)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    uint16_t idItem = jugador->quitar_item_de_slot(cmd.indiceItem);

    if (idItem == 0) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!catalogo.existe(idItem)) {
        jugador->agregar_item(idItem);
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!mapa.agregarItem(posicion, idItem)) {
        jugador->agregar_item(idItem);
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    std::list<EventoSalida> mensajes = {armarInventario(idCliente, *jugador)};

    mensajes.splice(mensajes.end(), armarItemEnSueloParaMapa(posicion, idItem));
    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarEquipar(uint16_t idCliente, const ComandoEquipar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (!jugador->equipar_item(cmd.indiceItem, catalogo)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    return {armarInventario(idCliente, *jugador), armarEquipamiento(idCliente, *jugador),
            armarEstado(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarComprar(uint16_t idCliente, const ComandoComprar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    // El cliente nos dice exactamente con qué NPC quiere transar (cmd.idNPC).
    // Puede ser Comerciante o Sacerdote. 
    // Probamos ambos tipos: si el id no matchea ninguno o está fuera de rango, error.
    std::pair<bool, uint8_t> resultadoVenta = {false, 0};
    bool npcEncontrado = false;

    if (Comerciante* comerciante = obtenerComercianteParaInteraccion(cmd.idNPC, *jugador)) {
        resultadoVenta = comerciante->venderItem(cmd.idItem);
        npcEncontrado = true;
    } else if (Sacerdote* sacerdote = obtenerSacerdoteParaInteraccion(cmd.idNPC, *jugador)) {
        resultadoVenta = sacerdote->venderItem(cmd.idItem);
        npcEncontrado = true;
    }

    if (!npcEncontrado) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    const bool puedeComprar = resultadoVenta.first;
    if (!puedeComprar) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    const uint8_t precioCompra = resultadoVenta.second;

    if (jugador->getOro() < precioCompra) {
        return {armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE)};
    }

    if (!jugador->agregar_item(cmd.idItem)) {
        return {armarError(idCliente, CodigoErrorAccion::INVENTARIO_LLENO)};
    }

    jugador->gastar_oro(precioCompra);

    return {armarInventario(idCliente, *jugador), armarEstado(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarVender(uint16_t idCliente, const ComandoVender& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Comerciante* comerciante = obtenerComercianteParaInteraccion(cmd.idNPC, *jugador);
    if (comerciante == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Peek antes de quitar: si el slot está vacío o el item no existe en el
    // catálogo, devolvemos error sin tocar el inventario.
    const uint16_t idItemEnSlot = jugador->getIdItemEnSlot(cmd.indiceItem);
    if (idItemEnSlot == 0 || !catalogo.existe(idItemEnSlot)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    const std::pair<bool, uint8_t> resultadoCompra = comerciante->comprarItem(idItemEnSlot);
    if (!resultadoCompra.first) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    // Validaciones pasadas: ahora sí mutamos.
    jugador->quitar_item_de_slot(cmd.indiceItem);
    const uint8_t precioVenta = resultadoCompra.second;
    jugador->sumar_oro(precioVenta);

    return {armarInventario(idCliente, *jugador), armarEstado(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarDepositarItem(uint16_t idCliente,
                                                     const ComandoDepositarItem& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Banquero* banquero = obtenerBanqueroParaInteraccion(cmd.idBanquero, *jugador);
    if (banquero == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Peek antes de tocar el inventario: si el slot está vacío o tiene un id que no existe en el catálogo, devolvemos error sin mutar nada.
    // Esto evita el ensuciar la cuenta del banco con id 0 o ítems fantasma, y elimina la necesidad de rollback.
    const uint16_t idItemEnSlot = jugador->getIdItemEnSlot(cmd.indiceItem);
    if (idItemEnSlot == 0 || !catalogo.existe(idItemEnSlot)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Una vez validado, sí mutamos: quitar del inventario y depositar. depositarItem ya rechaza id 0
    const uint16_t idItemDepositado = jugador->quitar_item_de_slot(cmd.indiceItem);
    if (!banquero->depositarItem(jugador->getId(), idItemDepositado)) {
        // Camino teórico: peek dijo válido y depositar falló. Rollback por seguridad para no perder el ítem.
        jugador->agregar_item_en_slot(idItemDepositado, cmd.indiceItem);
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }
    return {armarInventario(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarDepositarOro(uint16_t idCliente,
                                                    const ComandoDepositarOro& cmd) {

    Jugador* jugador = buscarJugador(idCliente);
    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Banquero* banquero = obtenerBanqueroParaInteraccion(cmd.idBanquero, *jugador);
    if (banquero == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (cmd.monto == 0) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (jugador->getOro() < cmd.monto) {
        return {armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE)};
    }

    if (!jugador->gastar_oro(cmd.monto)) {
        return {armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE)};
    }

    if (!banquero->depositarOro(jugador->getId(), cmd.monto)) {
        jugador->sumar_oro(cmd.monto);
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    return {armarEstado(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarRetirarItem(uint16_t idCliente,
                                                   const ComandoRetirarItem& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Banquero* banquero = obtenerBanqueroParaInteraccion(cmd.idBanquero, *jugador);
    if (banquero == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    // Validamos primero ambas precondiciones (item existe en banco + cabe en inventario) y RECIÉN AHÍ aplicamos la mutación. 
    
    if (cmd.idItem == 0 || !catalogo.existe(cmd.idItem)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!banquero->tieneItem(jugador->getId(), cmd.idItem)) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!jugador->puede_agregar_item(cmd.idItem)) {
        // El ítem sigue en el banco: todavía no mutamos ninguna colección.
        return {armarError(idCliente, CodigoErrorAccion::INVENTARIO_LLENO)};
    }

    // Primero sacamos del banco y luego agregamos al inventario. Como ya validamos capacidad, `agregar_item` debería aceptar; si no, hacemos rollback al banco para no perder ni duplicar el ítem.
    if (!banquero->retirarItem(jugador->getId(), cmd.idItem)) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (!jugador->agregar_item(cmd.idItem)) {
        banquero->depositarItem(jugador->getId(), cmd.idItem);
        return {armarError(idCliente, CodigoErrorAccion::INVENTARIO_LLENO)};
    }

    return {armarInventario(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarRetirarOro(uint16_t idCliente,
                                                  const ComandoRetirarOro& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Banquero* banquero = obtenerBanqueroParaInteraccion(cmd.idBanquero, *jugador);
    if (banquero == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (cmd.monto == 0) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (!jugador->puede_recibir_oro(cmd.monto)) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (!banquero->retirarOro(jugador->getId(), cmd.monto)) {
        return {armarError(idCliente, CodigoErrorAccion::ORO_INSUFICIENTE)};
    }

    jugador->sumar_oro(cmd.monto);

    return {armarEstado(idCliente, *jugador)};
}

std::list<EventoSalida> Juego::ejecutarListar(uint16_t idCliente, const ComandoListar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    if (Comerciante* comerciante = obtenerComercianteParaInteraccion(cmd.idNPC, *jugador)) {
        std::vector<uint16_t> ids;
        for (const auto& [idItem, precios] : comerciante->listarItemsDisponibles()) {
            ids.push_back(idItem);
        }

        return {EventoSalida{TipoDestino::UNO, idCliente, EventoListaItems{ids}}};
    }

    if (Sacerdote* sacerdote = obtenerSacerdoteParaInteraccion(cmd.idNPC, *jugador)) {
        std::vector<uint16_t> ids;
        for (const auto& [idItem, precio] : sacerdote->listarItemsDisponibles()) {
            ids.push_back(idItem);
        }

        return {EventoSalida{TipoDestino::UNO, idCliente, EventoListaItems{ids}}};
    }

    if (obtenerBanqueroParaInteraccion(cmd.idNPC, *jugador) != nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
}

std::list<EventoSalida> Juego::ejecutarCurar(uint16_t idCliente, const ComandoCurar& cmd) {
    Jugador* jugador = buscarJugador(idCliente);

    if (!jugador || !jugador->estaVivo()) {
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    Sacerdote* sacerdote = obtenerSacerdoteParaInteraccion(cmd.idSacerdote, *jugador);
    if (sacerdote == nullptr) {
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    jugador->curar(jugador->getVidaMax());
    jugador->recuperar_mana(jugador->getManaMax());
    return {armarEstado(idCliente, *jugador)};
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

        if (jugadorCercano.has_value() && jugadorCercano->estaVivo()) {
            std::list<EventoSalida> mensajesAtaque =
                    moverCriaturaHacia(*criatura, jugadorCercano->getPosicion());

            mensajes.splice(mensajes.end(), mensajesAtaque);
        } else {
            std::list<EventoSalida> mensajesMovimiento = moverCriaturaAleatoriamente(*criatura);
            mensajes.splice(mensajes.end(), mensajesMovimiento);
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

std::list<EventoSalida> Juego::moverCriaturaAleatoriamente(const Criatura& criatura) {
    std::vector<Posicion> destinosValidos;

    for (const Posicion& destino : calcularDestinosAdyacentes(criatura.getPos())) {
        if (puedeMoverCriaturaA(destino)) {
            destinosValidos.push_back(destino);
        }
    }

    if (destinosValidos.empty()) {
        return {};
    }

    const size_t indiceDestino = aleatorio.enteroEnRango<size_t>(0, destinosValidos.size() - 1);
    const Posicion destino = destinosValidos[indiceDestino];
    if (!mapa.moverCriatura(criatura.getId(), destino)) {
        return {};
    }

    const Criatura* criaturaActualizada = mapa.obtenerCriaturaPor(criatura.getId());
    if (criaturaActualizada == nullptr) {
        return {};
    }

    return armarPosicionCriaturaParaMapa(*criaturaActualizada);
}

std::list<EventoSalida> Juego::moverCriaturaHacia(const Criatura& criatura,
                                                  const Posicion& objetivo) {
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
                const Criatura* criaturaActualizada = mapa.obtenerCriaturaPor(criatura.getId());
                if (criaturaActualizada == nullptr) {
                    return {};
                }
                return armarPosicionCriaturaParaMapa(*criaturaActualizada);
            }
        }
    }

    return moverCriaturaAleatoriamente(criatura);
}

bool Juego::puedeMoverCriaturaA(const Posicion& destino) const {
    return mapa.puedeOcuparCriatura(destino) && !buscarIdJugadorEn(destino).has_value();
}

std::list<EventoSalida> Juego::atacarJugadorConCriatura(const Criatura& criatura,
                                                        uint16_t idJugador) {
    std::list<EventoSalida> mensajes;

    Jugador* jugador = buscarJugador(idJugador);

    if (!jugador || !jugador->estaVivo()) {
        return mensajes;
    }

    if (mapa.esZonaSegura(jugador->getPosicion()) || mapa.esZonaSegura(criatura.getPos())) {
        return mensajes;
    }

    // Las criaturas no tienen crítico
    // Pasamos esCritico=false explícito para que el defensor pueda evaluar evasión normalmente.
    const uint16_t danioBrutoCriatura = criatura.calcularDanio(aleatorio);
    const ResultadoDefensa resultadoDefensa = jugador->recibir_ataque_fisico(
            danioBrutoCriatura, /*esCritico=*/false, catalogo, aleatorio,
            multiplicadorClan(*jugador));

    // Jugador sale de estado de meditando al ser atacado, si estaba meditanto.
    jugador->cancelarMeditacion();
    mensajes.push_back(armarEstado(idJugador, *jugador));

    if (resultadoDefensa.tipo == ResultadoDefensa::Tipo::Esquivo) {
        mensajes.push_back(EventoSalida{TipoDestino::UNO, idJugador,
                                        EventoEsquive{idJugador, /*esquivador=*/1}});
        return mensajes;
    }

    const uint16_t danio = resultadoDefensa.danioAplicado;

    mensajes.push_back(EventoSalida{TipoDestino::UNO, idJugador,
                                    EventoDanioRecibido{danio, criatura.getId()}});

    mensajes.push_back(armarEstado(idJugador, *jugador));

    if (danio > 0 && jugador->tieneClan()) {
        mensajes.splice(mensajes.end(), armarEventoClanParaMiembrosOnline(
                                                jugador->getClan(), TipoEventoClan::BajoAtaque,
                                                jugador->getNombre(), idJugador));
    }

    if (!jugador->estaVivo()) {
        mensajes.splice(mensajes.end(), emitirMuerteJugador(*jugador, jugador->getPosicion()));
    }

    return mensajes;
}

template <typename TipoNpcConcreto>
static TipoNpcConcreto* obtenerNpcParaInteraccionEnMapa(
        Mapa& mapa,
        const ConfigJuego& cfg,
        uint16_t idNpc,
        const Jugador& jugador,
        TipoNpcConcreto* (Mapa::*obtenerNpc)(uint16_t)) {
    TipoNpcConcreto* npc = (mapa.*obtenerNpc)(idNpc);
    if (npc == nullptr) {
        return nullptr;
    }

    const Posicion posicionJugador = jugador.getPosicion();
    const Posicion posicionNpc = npc->getPosicion();

    if (!posicionJugador.mismaMapa(posicionNpc)) {
        return nullptr;
    }

    if (posicionJugador.distanciaManhattan(posicionNpc) > cfg.rangoInteraccionNpc) {
        return nullptr;
    }

    return npc;
}

Comerciante* Juego::obtenerComercianteParaInteraccion(uint16_t idNpc, const Jugador& jugador) {
    return obtenerNpcParaInteraccionEnMapa<Comerciante>(
            mapa, cfg, idNpc, jugador, &Mapa::obtenerComerciante);
}

Sacerdote* Juego::obtenerSacerdoteParaInteraccion(uint16_t idNpc, const Jugador& jugador) {
    return obtenerNpcParaInteraccionEnMapa<Sacerdote>(
            mapa, cfg, idNpc, jugador, &Mapa::obtenerSacerdote);
}

Banquero* Juego::obtenerBanqueroParaInteraccion(uint16_t idNpc, const Jugador& jugador) {
    return obtenerNpcParaInteraccionEnMapa<Banquero>(
            mapa, cfg, idNpc, jugador, &Mapa::obtenerBanquero);
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

std::optional<uint16_t> Juego::reservarIdCriatura() {
    static constexpr uint16_t ID_MINIMO_CRIATURA = 1;
    static constexpr uint16_t ID_MAXIMO_CRIATURA = std::numeric_limits<uint16_t>::max();

    for (uint32_t intentos = 0; intentos < ID_MAXIMO_CRIATURA; ++intentos) {
        const uint16_t candidato = proximoIdCriatura;

        if (proximoIdCriatura == ID_MINIMO_CRIATURA) {
            proximoIdCriatura = ID_MAXIMO_CRIATURA;
        } else {
            proximoIdCriatura--;
        }

        if (candidato == 0 || mapa.obtenerCriaturaPor(candidato) != nullptr ||
            existeIdPersonaje(candidato)) {
            continue;
        }

        return candidato;
    }

    return std::nullopt;
}

bool Juego::puedeSpawnearCriaturaEn(const Posicion& posicion) const {
    return mapa.puedeOcuparCriatura(posicion) && !mapa.hayItemEn(posicion) &&
           !mapa.hayOroEn(posicion) && !buscarIdJugadorEn(posicion).has_value();
}

std::optional<Posicion> Juego::buscarPosicionSpawnCriatura() {
    const uint64_t cantidadCeldas =
            static_cast<uint64_t>(cfg.mapaAncho) * static_cast<uint64_t>(cfg.mapaAlto);

    if (cantidadCeldas == 0) {
        return std::nullopt;
    }

    const uint64_t inicio = aleatorio.enteroEnRango<uint64_t>(0, cantidadCeldas - 1);

    for (uint64_t offset = 0; offset < cantidadCeldas; ++offset) {
        const uint64_t indice = (inicio + offset) % cantidadCeldas;
        const uint16_t x = static_cast<uint16_t>(indice % cfg.mapaAncho);
        const uint16_t y = static_cast<uint16_t>(indice / cfg.mapaAncho);
        const Posicion posicion{x, y, 0};

        if (puedeSpawnearCriaturaEn(posicion)) {
            return posicion;
        }
    }

    return std::nullopt;
}

std::list<EventoSalida> Juego::intentarSpawnCriatura() {
    std::list<EventoSalida> mensajes;

    std::optional<Posicion> posicion = buscarPosicionSpawnCriatura();
    if (!posicion.has_value()) {
        return mensajes;
    }

    std::optional<uint16_t> idCriatura = reservarIdCriatura();
    if (!idCriatura.has_value()) {
        return mensajes;
    }

    static constexpr std::array<TipoCriatura, 6> tiposCriatura = {
            TipoCriatura::Goblin, TipoCriatura::Esqueleto, TipoCriatura::Zombie,
            TipoCriatura::Arania, TipoCriatura::Orco,      TipoCriatura::Golem};

    const size_t indiceTipo = aleatorio.enteroEnRango<size_t>(0, tiposCriatura.size() - 1);

    const uint8_t danioMaximo = cfg.criaturaDanioMaxBase < cfg.criaturaDanioMinBase
                                        ? cfg.criaturaDanioMinBase
                                        : cfg.criaturaDanioMaxBase;
    const uint16_t cuerpoCriatura = getIndiceCuerpoCriatura(tiposCriatura[indiceTipo]);
    
    Criatura criatura(*idCriatura, tiposCriatura[indiceTipo],
                      cfg.criaturaVidaMaximaBase, cfg.criaturaNivelBase, cfg.criaturaFuerzaBase,
                      cfg.criaturaAgilidadBase, *posicion, cfg.criaturaRangoAggroBase,
                      cfg.criaturaDanioMinBase, danioMaximo, cuerpoCriatura);

    if (!agregarCriatura(criatura)) {
        return mensajes;
    }

    mensajes.splice(mensajes.end(), armarPosicionCriaturaParaMapa(criatura));
    return mensajes;
}

std::list<EventoSalida> Juego::armarOroEnSueloParaMapa(const Posicion& posicion,
                                                       uint32_t cantidad) {
    std::list<EventoSalida> mensajes;
    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back({TipoDestino::UNO, idCliente,
                                EventoOroEnSuelo{cantidad, posicion.x, posicion.y}});
        }
    }
    return mensajes;
}

std::list<EventoSalida> Juego::armarOroDesaparecioSueloParaMapa(const Posicion& posicion) {
    std::list<EventoSalida> mensajes;
    for (const auto& [idCliente, jugador] : jugadoresConectados) {
        if (jugador.getPosicion().mapaId == posicion.mapaId) {
            mensajes.push_back({TipoDestino::UNO, idCliente,
                                EventoOroDesaparecioSuelo{posicion.x, posicion.y}});
        }
    }
    return mensajes;
}

bool Juego::dropearOroEnSueloCercano(const Posicion& origen, uint32_t cantidad,
                                     Posicion& posicionFinal) {
    if (mapa.agregarOroEnSuelo(origen, cantidad)) {
        posicionFinal = origen;
        return true;
    }

    // Probamos en celdas adyacentes válidas. Esto es útil si la celda del NPC queda bloqueada por una pared post-spawn o por un ítem ya tirado.
    for (const Posicion& celdaCandidata : calcularDestinosAdyacentes(origen)) {
        if (mapa.agregarOroEnSuelo(celdaCandidata, cantidad)) {
            posicionFinal = celdaCandidata;
            return true;
        }
    }

    return false;
}

std::list<EventoSalida> Juego::procesarDropsJugadorMuerto(Jugador& jugador,
                                                          const Posicion& posicionMuerte) {
    std::list<EventoSalida> mensajes;

    const uint32_t oroDrop = jugador.extraer_oro_perdido();
    if (oroDrop > 0) {
        Posicion posicionDrop = posicionMuerte;
        if (dropearOroEnSueloCercano(posicionMuerte, oroDrop, posicionDrop)) {
            mensajes.splice(mensajes.end(), armarOroEnSueloParaMapa(posicionDrop, oroDrop));
        }
    }

    const std::vector<uint16_t> itemsDropear = jugador.vaciar_inventario();

    for (uint16_t idItem : itemsDropear) {
        Posicion posicionDrop = posicionMuerte;
        if (agregarItemEnSueloCercano(posicionMuerte, idItem, posicionDrop)) {
            mensajes.splice(mensajes.end(), armarItemEnSueloParaMapa(posicionDrop, idItem));
        }
    }

    const std::optional<uint16_t> idCliente = buscarIdClienteDeJugador(jugador.getId());
    if (idCliente.has_value()) {
        mensajes.push_back(armarInventario(*idCliente, jugador));
    }
    return mensajes;
}

std::list<EventoSalida> Juego::emitirMuerteJugador(Jugador& victima,
                                                   const Posicion& posicionMuerte) {
    std::list<EventoSalida> mensajes;
    const EventoMuerteEntidad eventoMuerte{victima.getId()};

    for (const auto& [idOtro, otroJugador] : jugadoresConectados) {
        if (otroJugador.getPosicion().mapaId == posicionMuerte.mapaId) {
            mensajes.push_back(EventoSalida{TipoDestino::UNO, idOtro, eventoMuerte});
        }
    }

    mensajes.splice(mensajes.end(), procesarDropsJugadorMuerto(victima, posicionMuerte));
    mensajes.splice(mensajes.end(), armarPosicionParaMapa(victima));

    return mensajes;
}

std::list<EventoSalida> Juego::ejecutarAtaqueACriatura(uint16_t idCliente, Jugador& atacanteRef,
                                                       Criatura& criatura) {
    Jugador* atacante = &atacanteRef;

    const Posicion posicionAtacante = atacante->getPosicion();
    const Posicion posicionCriatura = criatura.getPos();

    if (!posicionAtacante.mismaMapa(posicionCriatura)) {
        std::cerr << "Error: atacante y criatura no están en el mismo mapa." << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    if (mapa.esZonaSegura(posicionAtacante) || mapa.esZonaSegura(posicionCriatura)) {
        std::cerr << "Error: atacante o criatura están en zona segura." << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    // El alcance depende del equipamiento del atacante.
    // Si es hechizo, el atacante debe tener maná suficiente.
    const DescriptorAtaque descriptorAtaque = atacante->describir_ataque(catalogo);

    if (descriptorAtaque.tipo == TipoAtaque::HechizoNoOfensivo) {
        std::cerr << "Error: el atacante intenta usar un hechizo no ofensivo." << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::ACCION_NO_PERMITIDA)};
    }

    const int distanciaACriatura = posicionAtacante.distanciaManhattan(posicionCriatura);
    if (distanciaACriatura > descriptorAtaque.alcanceMaximo) {
        std::cerr << "Error: el objetivo está fuera del alcance del ataque." << std::endl;
        return {armarError(idCliente, CodigoErrorAccion::OBJETIVO_INVALIDO)};
    }

    bool atacanteConsumioMana = false;
    if (descriptorAtaque.costoMana > 0) {
        if (!atacante->consumir_mana(descriptorAtaque.costoMana)) {
            std::cerr << "Error: el atacante no tiene mana suficiente." << std::endl;
            return {armarError(idCliente, CodigoErrorAccion::MANA_INSUFICIENTE)};
        }
        atacanteConsumioMana = true;
    }

    // Snapshot pre-golpe: niveles y vida máxima sobreviven aunque la criatura muera.
    const uint8_t nivelAtacante = atacante->getNivel();
    const uint8_t nivelCriaturaAntes = criatura.getNivel();
    const uint16_t vidaMaximaCriaturaAntes = criatura.getVidaMaxima();
    const uint16_t vidaActualCriaturaAntes = criatura.getVidaActual();

    // PvE: la criatura no porta armadura/casco/escudo. Por lo tanto Defensa = 0 y el daño final es el bruto saturado al pool de vida actual. Calculamos primero para
    // conocer el flag de crítico antes de decidir si la criatura puede esquivar.
    const ResultadoDanio resultadoDanio = atacante->calcular_danio(catalogo, aleatorio);
    const uint16_t danioBruto = ReglasJuego::aplicarMultiplicadorCombate(
            resultadoDanio.valor, multiplicadorClan(*atacante));

    std::list<EventoSalida> mensajes;

    // El crítico omite la fase de evasión incluso en PvE.
    // Si el ataque no es crítico, el defensor (la criatura) puede esquivar. Fórmula centralizada en ReglasJuego 

    if (!resultadoDanio.esCritico) {
        const float valorAleatorioEsquive = aleatorio.uniforme();
        const bool criaturaEsquiva =
                ReglasJuego::esquivaAtaque(cfg, criatura.getAgilidad(), valorAleatorioEsquive);

        if (criaturaEsquiva) {
            mensajes.push_back(EventoSalida{TipoDestino::UNO, idCliente,
                                            EventoEsquive{criatura.getId(), /*esquivador=*/1}});
            return mensajes;
        }
    }

    const uint16_t danioAplicado =
            static_cast<uint16_t>(std::min<uint32_t>(danioBruto, vidaActualCriaturaAntes));
    criatura.recibir_danio(danioBruto);
    std::cout << "El jugador con id " << atacante->getId() << " ataca a criatura con id " << criatura.getId() << std::endl;

    mensajes.push_back(EventoSalida{TipoDestino::UNO, idCliente,
                                    EventoDanioProducido{danioAplicado, criatura.getId()}});

    // XP por impacto
    bool atacanteGanoXp = false;
    if (danioAplicado > 0) {
        const uint32_t xpHit = ReglasJuego::calcularExperienciaImpacto(
                cfg, danioAplicado, nivelAtacante, nivelCriaturaAntes);
        if (xpHit > 0) {
            atacante->ganar_experiencia(xpHit);
            atacanteGanoXp = true;
        }
    }

    if (criatura.esta_muerta()) {
        const uint16_t idCriatura = criatura.getId();

        // XP por kill
        const float valorAleatorioXpKill = aleatorio.uniforme();
        const uint32_t xpKill =
                ReglasJuego::calcularExperienciaKill(cfg, vidaMaximaCriaturaAntes, nivelAtacante,
                                                     nivelCriaturaAntes, valorAleatorioXpKill);
        if (xpKill > 0) {
            atacante->ganar_experiencia(xpKill);
            atacanteGanoXp = true;
        }

        // Drop de oro. Muestra independiente del rng.
        const float valorAleatorioDropOro = aleatorio.uniforme();
        const uint32_t cantidadOro = ReglasJuego::calcularDropOroNpc(cfg, vidaMaximaCriaturaAntes,
                                                                     valorAleatorioDropOro);

        // Eliminar la criatura del mapa ANTES de buscar celda libre para el oro, así la propia celda del NPC pasa a ser candidata válida.
        mapa.removerCriatura(idCriatura);
        std::cout << "La criatura con id " << idCriatura << " ha muerto" << std::endl;

        if (cantidadOro > 0) {
            Posicion celdaDrop = posicionCriatura;
            if (dropearOroEnSueloCercano(posicionCriatura, cantidadOro, celdaDrop)) {
                mensajes.splice(mensajes.end(), armarOroEnSueloParaMapa(celdaDrop, cantidadOro));
            }
        }

        // Notificar la muerte a todos los jugadores en el mismo mapa para que
        // limpien la entidad de su escena.
        const EventoMuerteEntidad eventoMuerte{idCriatura};
        for (const auto& [idOtro, otroJugador] : jugadoresConectados) {
            if (otroJugador.getPosicion().mapaId == posicionCriatura.mapaId) {
                mensajes.push_back(EventoSalida{TipoDestino::UNO, idOtro, eventoMuerte});
            }
        }
    }

    // Emitimos estado del atacante si cambió algo visible: XP/nivel ganado o maná consumido por hechizo.
    if (atacanteGanoXp || atacanteConsumioMana) {
        mensajes.push_back(armarEstado(idCliente, *atacante));
    }

    return mensajes;
}

uint16_t Juego::getIndiceCuerpoCriatura(TipoCriatura tipo) const {
    switch (tipo) {
        case TipoCriatura::Goblin:
            return 2200;
        case TipoCriatura::Esqueleto:
            return 2201;
        case TipoCriatura::Zombie:
            return 2202;
        case TipoCriatura::Arania:
            return 2203;
        case TipoCriatura::Orco:
            return 2204;
        case TipoCriatura::Golem:
            return 2205;
        default:
            return 0;
    }
}
