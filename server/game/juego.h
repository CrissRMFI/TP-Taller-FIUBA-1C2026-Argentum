#ifndef JUEGO_H
#define JUEGO_H

#include <cstdint>
#include <list>
#include <map>
#include <string>

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/mensajes/codigo_error_accion.h"
#include "../gameloop/mensaje_salida.h"
#include "clan.h"
#include "config/config_juego.h"
#include "jugador.h"
#include "criatura.h"
#include "objeto/catalogo_items.h"

class Juego {
  public:
    Juego(const ConfigJuego& cfg, CatalogoItems catalogo);

    void conectarJugador(uint16_t id, const std::string& nombre,
                         ClasePersonaje clase, Raza raza, Posicion posicion);
    void desconectarJugador(uint16_t id);

    std::list<MensajeSalida> ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando);
    std::list<MensajeSalida> actualizar();

  private:
    ConfigJuego   cfg;
    CatalogoItems catalogo;
    uint16_t      proximoIdClan;
    std::map<uint16_t, Clan>     clanes;
    std::map<uint16_t, Jugador>  jugadoresConectados;
    std::map<uint16_t, Jugador>  jugadoresDesconectados;
    std::map<uint16_t, Criatura> criaturasEnMapa;

    // Búsqueda
    Jugador*    buscarJugador(uint16_t id);
    Jugador*    buscarJugadorPorNick(const std::string& nick);
    Clan*       buscarClanPorNombre(const std::string& nombre);
    std::string nickDe(uint16_t idJugador);

    // Construcción de mensajes comunes
    MensajeSalida armarError(uint16_t idCliente, CodigoErrorAccion cod);
    MensajeSalida armarEstado(uint16_t idCliente, const Jugador& j);
    MensajeSalida armarInventario(uint16_t idCliente, const Jugador& jugador);
    MensajeSalida armarEquipamiento(uint16_t idCliente, const Jugador& jugador);

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
};

#endif
