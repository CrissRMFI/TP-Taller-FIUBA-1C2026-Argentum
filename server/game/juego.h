#ifndef JUEGO_H
#define JUEGO_H

#include <cstdint>
#include <list>

#include "../../common/protocolo/comando_jugador.h"
#include "../gameloop/mensaje_salida.h"
#include "clan.h"
#include "jugador.h"

class Juego {
  public:
    std::list<MensajeSalida> ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando);
  
 private:
    std::list<Clan> clanes;
    std::list<Jugador> jugadoresConectados;
    std::list<Jugador> jugadoresDesconectados; // Para manejar desconexiones y reconexiones
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
    std::list<MensajeSalida> ejecutarDepositarItem(uint16_t idCliente,const ComandoDepositarItem& comando);
    std::list<MensajeSalida> ejecutarDepositarOro(uint16_t idCliente,const ComandoDepositarOro& comando);
    std::list<MensajeSalida> ejecutarRetirarItem(uint16_t idCliente,const ComandoRetirarItem& comando);
    std::list<MensajeSalida> ejecutarRetirarOro(uint16_t idCliente,const ComandoRetirarOro& comando);
    std::list<MensajeSalida> ejecutarListar(uint16_t idCliente, const ComandoListar& comando);
    std::list<MensajeSalida> ejecutarCurar(uint16_t idCliente, const ComandoCurar& comando);
    std::list<MensajeSalida> ejecutarChatGlobal(uint16_t idCliente,const ComandoChatGlobal& comando);
    std::list<MensajeSalida> ejecutarChatPrivado(uint16_t idCliente,const ComandoChatPrivado& comando);
    std::list<MensajeSalida> ejecutarFundarClan(uint16_t idCliente,const ComandoFundarClan& comando);
    std::list<MensajeSalida> ejecutarUnirseClan(uint16_t idCliente,const ComandoUnirseClan& comando);
    std::list<MensajeSalida> ejecutarGestionMiembroClan(uint16_t idCliente,const ComandoGestionMiembreClan& comando);
};

#endif
