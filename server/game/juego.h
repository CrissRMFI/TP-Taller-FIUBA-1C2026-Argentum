#ifndef JUEGO_H
#define JUEGO_H

#include <cstddef>
#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../../common/protocolo/comando_jugador.h"
#include "../../common/mensajes/codigo_error_accion.h"
#include "aleatorio.h"
#include "clan.h"
#include "config/config_juego.h"
#include "evento/evento_salida.h"
#include "jugador.h"
#include "criatura.h"
#include "mapa/mapa.h"
#include "objeto/catalogo_items.h"

class Juego {
  public:
    Juego(const ConfigJuego& cfg, CatalogoItems&& catalogo);

    std::list<EventoSalida> conectarJugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion);
    std::list<EventoSalida> desconectarJugador(uint16_t id);

    std::list<EventoSalida> ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando);
    std::list<EventoSalida> actualizar(float deltaSegundos);

  private:
    ConfigJuego   cfg;
    CatalogoItems catalogo;
    uint16_t      proximoIdClan;
    uint16_t      proximoIdCriatura;
    std::map<uint16_t, Clan>     clanes;
    std::unordered_map<uint16_t, Jugador> jugadoresConectados;
    std::unordered_map<uint16_t, Jugador> jugadoresDesconectados;
    std::unordered_map<std::string, uint16_t> indiceNicksConectados;
    Mapa mapa;
    uint64_t ticksTranscurridos;
    Aleatorio aleatorio;

    // Búsqueda
    Jugador*    buscarJugador(uint16_t id);
    Jugador*    buscarJugadorPorIdPersonaje(uint16_t idPersonaje);
    std::optional<uint16_t> buscarIdClienteDeJugador(uint16_t idPersonaje) const;
    bool        existeIdPersonaje(uint16_t idPersonaje) const;
    Jugador*    buscarJugadorPorNick(const std::string& nick);
    Clan*       buscarClanPorNombre(const std::string& nombre);

    // Construcción de eventos comunes
    EventoSalida armarError(uint16_t idCliente, CodigoErrorAccion cod);
    EventoSalida armarEstado(uint16_t idCliente, const Jugador& j);
    EventoSalida armarInventario(uint16_t idCliente, const Jugador& jugador);
    EventoSalida armarEquipamiento(uint16_t idCliente, const Jugador& jugador);
    EventoSalida armarPosicionPara(uint16_t idCliente, const Jugador& jugador);
    EventoSalida armarPosicionCriaturaPara(uint16_t idCliente, const Criatura& criatura);
    std::list<EventoSalida> armarDesaparicionParaMapa(const Jugador& jugador);
    std::list<EventoSalida> armarPosicionParaMapa(const Jugador& jugador);
    std::list<EventoSalida> armarPosicionCriaturaParaMapa(const Criatura& criatura);
    std::list<EventoSalida> armarItemEnSueloParaMapa(const Posicion& posicion, uint16_t idItem);
    std::list<EventoSalida> armarItemDesaparecioSueloParaMapa(const Posicion& posicion);
    std::list<EventoSalida> armarEventoClanParaMiembrosOnline(
            uint16_t idClan,
            TipoEventoClan tipo,
            const std::string& texto,
            std::optional<uint16_t> idExcluido = std::nullopt) const;
    bool agregarCriatura(const Criatura& criatura);
    std::list<EventoSalida> intentarSpawnCriatura();
    std::optional<uint16_t> reservarIdCriatura();
    std::optional<Posicion> buscarPosicionSpawnCriatura();
    bool puedeSpawnearCriaturaEn(const Posicion& posicion) const;
    bool agregarItemEnSueloCercano(const Posicion& origen, uint16_t idItem, Posicion& posicionFinal);

    std::list<EventoSalida> ejecutarMeditar(uint16_t idCliente);
    std::list<EventoSalida> ejecutarResucitar(uint16_t idCliente);
    std::list<EventoSalida> ejecutarTomar(uint16_t idCliente);
    std::list<EventoSalida> ejecutarRevisarClan(uint16_t idCliente);
    std::list<EventoSalida> ejecutarDejarClan(uint16_t idCliente);
    std::list<EventoSalida> ejecutarMover(uint16_t idCliente, const ComandoMover& comando);
    std::list<EventoSalida> ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& comando);
    std::list<EventoSalida> ejecutarTirar(uint16_t idCliente, const ComandoTirar& comando);
    std::list<EventoSalida> ejecutarEquipar(uint16_t idCliente, const ComandoEquipar& comando);
    std::list<EventoSalida> ejecutarComprar(uint16_t idCliente, const ComandoComprar& comando);
    std::list<EventoSalida> ejecutarVender(uint16_t idCliente, const ComandoVender& comando);
    std::list<EventoSalida> ejecutarDepositarItem(uint16_t idCliente, const ComandoDepositarItem& comando);
    std::list<EventoSalida> ejecutarDepositarOro(uint16_t idCliente, const ComandoDepositarOro& comando);
    std::list<EventoSalida> ejecutarRetirarItem(uint16_t idCliente, const ComandoRetirarItem& comando);
    std::list<EventoSalida> ejecutarRetirarOro(uint16_t idCliente, const ComandoRetirarOro& comando);
    std::list<EventoSalida> ejecutarListar(uint16_t idCliente, const ComandoListar& comando);
    std::list<EventoSalida> ejecutarCurar(uint16_t idCliente, const ComandoCurar& comando);
    std::list<EventoSalida> ejecutarChatGlobal(uint16_t idCliente, const ComandoChatGlobal& comando);
    std::list<EventoSalida> ejecutarChatPrivado(uint16_t idCliente, const ComandoChatPrivado& comando);
    std::list<EventoSalida> ejecutarFundarClan(uint16_t idCliente, const ComandoFundarClan& comando);
    std::list<EventoSalida> ejecutarUnirseClan(uint16_t idCliente, const ComandoUnirseClan& comando);
    std::list<EventoSalida> ejecutarGestionMiembroClan(uint16_t idCliente, const ComandoGestionMiembreClan& comando, Opcode accion);

    std::optional<uint16_t> buscarIdJugadorEn(
            const Posicion& posicion,
            std::optional<uint16_t> idExcluido = std::nullopt) const;
    std::optional<Posicion> buscarPosicionLibreParaResurreccion(
            const Posicion& origen,
            std::optional<uint16_t> idJugadorExcluido = std::nullopt) const;
    size_t contarAliadosClanCercanos(const Jugador& jugador) const;
    float multiplicadorClan(const Jugador& jugador) const;
    std::list<EventoSalida> actualizarCriaturas();

    std::optional<Jugador> buscarJugadorCercano(const Criatura& criatura) const;
    std::vector<Posicion> calcularDestinosHacia(const Posicion& origen, const Posicion& objetivo) const;
    std::vector<Posicion> calcularDestinosAdyacentes(const Posicion& origen) const;
    std::list<EventoSalida> moverCriaturaAleatoriamente(const Criatura& criatura);
    std::list<EventoSalida> moverCriaturaHacia(const Criatura& criatura, const Posicion& objetivo);

    bool puedeMoverCriaturaA(const Posicion& destino) const;
    std::list<EventoSalida> atacarJugadorConCriatura(const Criatura& criatura, uint16_t idJugador);

    std::list<EventoSalida> ejecutarAtaqueAJugador(uint16_t idCliente, Jugador& atacante, const ComandoAtacar& comando);
    std::list<EventoSalida> ejecutarAtaqueACriatura(uint16_t idCliente, Jugador& atacante, Criatura& criatura);

    std::list<EventoSalida> armarOroEnSueloParaMapa(const Posicion& posicion, uint32_t cantidad);
    std::list<EventoSalida> armarOroDesaparecioSueloParaMapa(const Posicion& posicion);
    bool dropearOroEnSueloCercano(const Posicion& origen, uint32_t cantidad, Posicion& posicionFinal);
    std::list<EventoSalida> procesarDropsJugadorMuerto(Jugador& jugador, const Posicion& posicionMuerte);

    // Centraliza el broadcast de muerte de un jugador: notifica a los demás
    // jugadores del mismo mapa, procesa los drops y re-emite la posición con
    // el nuevo estado de fantasma. Reemplaza el mismo bloque triplicado en
    // actualizar(), ejecutarAtaqueAJugador() y atacarJugadorConCriatura().
    std::list<EventoSalida> emitirMuerteJugador(Jugador& victima, const Posicion& posicionMuerte);

    // Helpers de validación NPC-por-id (regla del bug #6): obtienen el NPC
    // concreto del mapa por el id que envió el cliente y verifican que esté
    // en el mismo mapa y dentro del rango de interacción. Devuelven nullptr
    // si el id no matchea, si el NPC está en otro mapa o si está fuera de
    // rango. `Juego` traduce el nullptr al `armarError` correspondiente.
    template <typename Npc>
    Npc* obtenerNpcParaInteraccion(uint16_t idNpc,
                                   const Jugador& jugador,
                                   Npc* (Mapa::*obtenerNpc)(uint16_t)) {
        Npc* npc = (mapa.*obtenerNpc)(idNpc);
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

    Comerciante* obtenerComercianteParaInteraccion(uint16_t idNpc, const Jugador& jugador);
    Sacerdote*   obtenerSacerdoteParaInteraccion  (uint16_t idNpc, const Jugador& jugador);
    Banquero*    obtenerBanqueroParaInteraccion   (uint16_t idNpc, const Jugador& jugador);
    };

#endif
