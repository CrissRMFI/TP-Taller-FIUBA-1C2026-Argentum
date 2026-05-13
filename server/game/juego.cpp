#include "juego.h"

#include <variant>
#include <list>
#include "../gameloop/mensaje_salida.h"

std::list<MensajeSalida> Juego::ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando) {

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
        return ejecutarDepositarItem(idCliente,std::get<ComandoDepositarItem>(comando.payload));

      case Opcode::DEPOSITAR_ORO:
        return ejecutarDepositarOro(idCliente,std::get<ComandoDepositarOro>(comando.payload));

      case Opcode::RETIRAR_ITEM:
        return ejecutarRetirarItem(idCliente,std::get<ComandoRetirarItem>(comando.payload));

      case Opcode::RETIRAR_ORO:
        return ejecutarRetirarOro(idCliente,std::get<ComandoRetirarOro>(comando.payload));

      case Opcode::LISTAR:
        return ejecutarListar(idCliente, std::get<ComandoListar>(comando.payload));

      case Opcode::CURAR:
        return ejecutarCurar(idCliente, std::get<ComandoCurar>(comando.payload));

      case Opcode::CHAT_GLOBAL:
        return ejecutarChatGlobal(idCliente,std::get<ComandoChatGlobal>(comando.payload));

      case Opcode::CHAT_PRIVADO:
        return ejecutarChatPrivado(idCliente,std::get<ComandoChatPrivado>(comando.payload));

      case Opcode::FUNDAR_CLAN:
        return ejecutarFundarClan(idCliente,std::get<ComandoFundarClan>(comando.payload));

      case Opcode::UNIRSE_CLAN:
        return ejecutarUnirseClan(idCliente,std::get<ComandoUnirseClan>(comando.payload));

      case Opcode::CLAN_ACEPTAR:
      case Opcode::CLAN_RECHAZAR:
      case Opcode::CLAN_BAN:
      case Opcode::CLAN_KICK:
        return ejecutarGestionMiembroClan(idCliente,std::get<ComandoGestionMiembreClan>(comando.payload));

      default:
        return {};
    }
}
