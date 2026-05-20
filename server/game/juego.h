#ifndef JUEGO_H
#define JUEGO_H

#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../../common/protocolo/comando_jugador.h"
#include "../../common/mensajes/codigo_error_accion.h"
#include "../gameloop/mensaje_salida.h"
#include "clan.h"
#include "config/config_juego.h"
#include "jugador.h"
#include "criatura.h"
#include "mapa/mapa.h"
#include "objeto/catalogo_items.h"

class Juego {
  public:
    Juego(const ConfigJuego& cfg, CatalogoItems&& catalogo);

    std::list<MensajeSalida> conectarJugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion);
    std::list<MensajeSalida> desconectarJugador(uint16_t id);

    std::list<MensajeSalida> ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando);
    std::list<MensajeSalida> actualizar(float deltaSegundos);

  private:
    ConfigJuego   cfg;
    CatalogoItems catalogo;
    uint16_t      proximoIdClan;
    std::map<uint16_t, Clan>     clanes;
    std::unordered_map<uint16_t, Jugador> jugadoresConectados;
    std::unordered_map<uint16_t, Jugador> jugadoresDesconectados;
    std::unordered_map<std::string, uint16_t> indiceNicksConectados;
    Mapa mapa;
    uint64_t ticksTranscurridos;

    // Búsqueda
    Jugador*    buscarJugador(uint16_t id);
    Jugador*    buscarJugadorPorNick(const std::string& nick);
    Clan*       buscarClanPorNombre(const std::string& nombre);

    // Construcción de mensajes comunes
    MensajeSalida armarError(uint16_t idCliente, CodigoErrorAccion cod);
    MensajeSalida armarEstado(uint16_t idCliente, const Jugador& j);
    MensajeSalida armarInventario(uint16_t idCliente, const Jugador& jugador);
    MensajeSalida armarEquipamiento(uint16_t idCliente, const Jugador& jugador);
    MensajeSalida armarPosicionPara(uint16_t idCliente, const Jugador& jugador);
    std::list<MensajeSalida> armarDesaparicionParaMapa(const Jugador& jugador);
    std::list<MensajeSalida> armarPosicionParaMapa(const Jugador& jugador);
    std::list<MensajeSalida> armarItemEnSueloParaMapa(const Posicion& posicion, uint16_t idItem);
    std::list<MensajeSalida> armarItemDesaparecioSueloParaMapa(const Posicion& posicion);
    bool agregarItemEnSueloCercano(const Posicion& origen, uint16_t idItem, Posicion& posicionFinal);

    std::list<uint16_t> criaturasCerca(Posicion posicionJugador);

    std::list<MensajeSalida> ejecutarMeditar(uint16_t idCliente);
    std::list<MensajeSalida> ejecutarResucitar(uint16_t idCliente);
    std::list<MensajeSalida> ejecutarTomar(uint16_t idCliente);
    std::list<MensajeSalida> ejecutarRevisarClan(uint16_t idCliente);
    std::list<MensajeSalida> ejecutarDejarClan(uint16_t idCliente);
    std::list<MensajeSalida> ejecutarMover(uint16_t idCliente, const ComandoMover& comando);
    std::list<MensajeSalida> ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& comando);
    std::list<MensajeSalida> ejecutarTirar(uint16_t idCliente, const ComandoTirar& comando);
    std::list<MensajeSalida> ejecutarEquipar(uint16_t idCliente, const ComandoEquipar& comando);
    std::list<MensajeSalida> ejecutarComprar(uint16_t idCliente, const ComandoComprar& comando);
    std::list<MensajeSalida> ejecutarVender(uint16_t idCliente, const ComandoVender& comando);
    std::list<MensajeSalida> ejecutarDepositarItem(uint16_t idCliente, const ComandoDepositarItem& comando);
    std::list<MensajeSalida> ejecutarDepositarOro(uint16_t idCliente, const ComandoDepositarOro& comando);
    std::list<MensajeSalida> ejecutarRetirarItem(uint16_t idCliente, const ComandoRetirarItem& comando);
    std::list<MensajeSalida> ejecutarRetirarOro(uint16_t idCliente, const ComandoRetirarOro& comando);
    std::list<MensajeSalida> ejecutarListar(uint16_t idCliente, const ComandoListar& comando);
    std::list<MensajeSalida> ejecutarCurar(uint16_t idCliente, const ComandoCurar& comando);
    std::list<MensajeSalida> ejecutarChatGlobal(uint16_t idCliente, const ComandoChatGlobal& comando);
    std::list<MensajeSalida> ejecutarChatPrivado(uint16_t idCliente, const ComandoChatPrivado& comando);
    std::list<MensajeSalida> ejecutarFundarClan(uint16_t idCliente, const ComandoFundarClan& comando);
    std::list<MensajeSalida> ejecutarUnirseClan(uint16_t idCliente, const ComandoUnirseClan& comando);
    std::list<MensajeSalida> ejecutarGestionMiembroClan(uint16_t idCliente, const ComandoGestionMiembreClan& comando, Opcode accion);

    bool posicionOcupadaPorJugador(uint16_t idCliente, const Posicion& posicion) const;
    bool posicionOcupadaPorAlgunJugador(const Posicion& posicion) const;
    std::list<MensajeSalida> actualizarCriaturas();

    std::optional<Jugador> buscarJugadorCercano(const Criatura& criatura) const;
    std::vector<Posicion> calcularDestinosHacia(const Posicion& origen, const Posicion& objetivo) const;
    std::vector<Posicion> calcularDestinosAdyacentes(const Posicion& origen) const;
    void moverCriaturaAleatoriamente(const Criatura& criatura);
    std::list<MensajeSalida> moverCriaturaHacia(const Criatura& criatura, const Posicion& objetivo);

    bool puedeMoverCriaturaA(const Posicion& destino) const;
    std::list<MensajeSalida> atacarJugadorConCriatura(const Criatura& criatura, uint16_t idJugador);
    std::optional<uint16_t> buscarIdJugadorEn(const Posicion& posicion) const;
};

#endif
