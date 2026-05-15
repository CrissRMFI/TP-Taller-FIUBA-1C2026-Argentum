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
        return ejecutarGestionMiembroClan(idCliente,std::get<ComandoGestionMiembreClan>(comando.payload), comando.opcode);

      default:
        return {};
    }
}

std::list<MensajeSalida> Juego::actualizar() {
//  - Regenerar HP y Maná de todos los jugadores (más rápido si están meditando)
//  - Hacer que los NPCs hostiles ataquen: 
//        usar criaturasCerca para cada jugador
//        si hay criaturas cerca, hacer que persigan al jugador (si no estan persiguiendo a otro(estaPersiguiendo())) y lo ataquen si están en rango
//  - Manejar el tiempo de respawn de las criaturas
//  - Hacer desaparecer ítems tirados en el suelo que hayan caducado
  return {};
}

std::list<uint16_t> Juego::criaturasCerca(Posicion posicionJugador) {
  // Devuelve una lista de IDs de criaturas que están cerca del jugador, para manejar el aggro de las criaturas
  return {};
}

std::list<MensajeSalida> Juego::ejecutarMeditar(uint16_t idCliente) {
  // Chequear si esta vivo y ponerlo a meditar
  return {};
}

std::list<MensajeSalida> Juego::ejecutarResucitar(uint16_t idCliente) {
  // Ver si el jugador sea fantasma y chequear:
  // - Si esta cerca de un sacerdote, resucitarlo en el lugar del sacerdote
  // - Si no, resucitarlo en la ciudad mas cercana
  return {};
}

std::list<MensajeSalida> Juego::ejecutarTomar(uint16_t idCliente) {
//  Checkear:
//    Está vivo (los fantasmas no pueden agarrar cosas).
//    Hay un ítem en su posición
//    Tiene espacio en el inventario

//   Hacer: 
//   Borrar el ítem del mapa y agregarlo al inventario del jugador
  return {};
}

std::list<MensajeSalida> Juego::ejecutarRevisarClan(uint16_t idCliente) {
// Chequear:
// - El jugador es lider de un clan
// Hacer:
// - Devolver la lista de miembros pendientes y actuales

  return {};
}

std::list<MensajeSalida> Juego::ejecutarDejarClan(uint16_t idCliente) {
  // El jugador deja su clan, solo si tiene clan y no es lider
  return {};
}

std::list<MensajeSalida> Juego::ejecutarMover(uint16_t idCliente, const ComandoMover& comando) {
  // Chequeos:
  // - Dentro de los limites del mapa
  // - Celda del mapa transitable
  // - Celda libre de otros jugadores

  // Hacer:
  // - Si esta meditando, sacarlo de ese estado
  // - Si esta resucitando, no dejarlo moverse
  // - Actualizar sus coordenadas (X,Y). 
  // - Liberar la celda vieja y ocupar la nueva en la matriz del mapa.
  return {};
}

std::list<MensajeSalida> Juego::ejecutarAtacar(uint16_t idCliente, const ComandoAtacar& comando) {
  //Checkear: 
  //    Esta vivo
  //    El objetivo:
  //      Está en rango de ataque (dependiendo del arma equipado)
  //      No es newbie
  //      No es miembro de mi clan
  //      No esta en una "Zona Segura"
  //      |Lvl_obj - Lvl_jugador| <= 10

  // Hacer: 
  //    Restar HP al objetivo.
  //    Si el objetivo muere: Cambiar su estado a Fantasma, dropear su oro exceso/items al suelo y sumarle experiencia al atacante

  // 
  return {};
}

std::list<MensajeSalida> Juego::ejecutarTirar(uint16_t idCliente, const ComandoTirar& comando) {
  // Checkear: 
  //    Está vivo (los fantasmas no pueden tirar cosas)
  //    Tiene el ítem en ese slot del inventario
  //    La celda actual (X,Y) del mapa está vacía de ítems (si se permite un item por celda)

  // Hacer: 
  //    Sacar el item del inventario
  //    Poner el item en la celda actual del mapa
  return {};
}

std::list<MensajeSalida> Juego::ejecutarEquipar(uint16_t idCliente, const ComandoEquipar& comando) {
  // Checkear: 
  //    Está vivo (los fantasmas no pueden equipar cosas)
  //    Tiene el item en el inventario
  //    Su Raza y Clase le permiten usarlo

  // Hacer: 
  //    Desequipar el ítem anterior de ese slot (si había uno).
  //    Marcar el nuevo ítem como equipado
  return {};
}

std::list<MensajeSalida> Juego::ejecutarComprar(uint16_t idCliente, const ComandoComprar& comando) {
//  Checkear:
//  - Está vivo
//  - Está a cerca de un Comerciante
//  - El NPC tiene el item en su lista de venta
//  - El jugador tiene suficiente oro
//  - El jugador tiene espacio en su inventario
//  Hacer:
//  - Restar el oro del jugador
//  - Agregar el item al inventario del jugador

  return {};
}

std::list<MensajeSalida> Juego::ejecutarVender(uint16_t idCliente, const ComandoVender& comando) {
//  Checkear:
//  - Está vivo
//  - Está cerca de un comerciante
//  - El jugador tiene el item en su inventario
//  Hacer:
//  - Agregar el oro al jugador
//  - Sacar el item del inventario del jugador

  return {};
}

std::list<MensajeSalida> Juego::ejecutarDepositarItem(uint16_t idCliente,const ComandoDepositarItem& comando) {
//  Checkear:
//  - Está vivo
//  - Está cerca de un banquero
//  - El jugador tiene el item en su inventario
//  - El jugador tiene espacio en su banco
//  Hacer:
//  - Sacar el item del inventario del jugador
//  - Agregar el item al banco del jugador

  return {};
}

std::list<MensajeSalida> Juego::ejecutarDepositarOro(uint16_t idCliente,const ComandoDepositarOro& comando) {
//  Checkear:
//  - Está vivo
//  - Está cerca de un banquero
//  - El jugador tiene el oro en su inventario
//  - El jugador tiene espacio en su banco
//  Hacer:
//  - Restar el oro del inventario del jugador
//  - Agregar el oro al banco del jugador
  return {};
}

std::list<MensajeSalida> Juego::ejecutarRetirarItem(uint16_t idCliente,const ComandoRetirarItem& comando) {
//  Checkear:
//  - Está vivo
//  - Está cerca de un banquero
//  - El jugador tiene el item en su banco
//  - El jugador tiene espacio en su inventario
//  Hacer:
//  - Sacar el item del banco del jugador
//  - Agregar el item al inventario del jugador
  return {};
}

std::list<MensajeSalida> Juego::ejecutarRetirarOro(uint16_t idCliente,const ComandoRetirarOro& comando) {
//  Checkear:
//  - Está vivo
//  - Está cerca de un banquero
//  - El jugador tiene el oro en su banco
//  Hacer:
//  - Sacar el oro del banco del jugador
//  - Agregar el oro al inventario del jugador
  return {};
}

std::list<MensajeSalida> Juego::ejecutarListar(uint16_t idCliente, const ComandoListar& comando) {
  // Checkear:
  //   Esta vivo
  //   Esta cerca de un comerciante o banquero
  // Hacer:
  //   Devolver la lista de items que tiene y su precio de compra en caso de que sea el comerciante
  return {};
}

std::list<MensajeSalida> Juego::ejecutarCurar(uint16_t idCliente, const ComandoCurar& comando) {
  // Chequear si esta vivo y cerca de un sacerdote
  return {};
}

std::list<MensajeSalida> Juego::ejecutarChatGlobal(uint16_t idCliente,const ComandoChatGlobal& comando) {
// Armar mensaje y hacer broadcast a todos los jugadores conectados
  return {};
}

std::list<MensajeSalida> Juego::ejecutarChatPrivado(uint16_t idCliente,const ComandoChatPrivado& comando) {
  // Chequear si el destinatario existe y esta conectado, armar mensaje y enviarlo solo a ese jugador
  return {};
}

std::list<MensajeSalida> Juego::ejecutarFundarClan(uint16_t idCliente,const ComandoFundarClan& comando) {
  // Chequear:
  // - El jugador no tiene clan
  // - El jugador es nivel 6 o superior
  // - El nombre del clan no esta tomado

  // Hacer:
  // - Crear el clan y agregarlo a la lista de clanes
  return {};
}

std::list<MensajeSalida> Juego::ejecutarUnirseClan(uint16_t idCliente,const ComandoUnirseClan& comando) {
  // Chequear:
  // - El jugador no tiene clan
  // - El clan al que quiere unirse existe
  // Hacer:
  // - Agregar al jugador a la lista de miembros pendientes del clan
  return {};
}

std::list<MensajeSalida> Juego::ejecutarGestionMiembroClan(uint16_t idCliente,const ComandoGestionMiembreClan& comando, Opcode accion) {
  switch (accion) {
    case Opcode::CLAN_ACEPTAR:
      // Chequear:
      // - El jugador es lider del clan
      // - El jugador a aceptar esta en la lista de miembros pendientes del clan
      // Hacer:
      // - Mover al jugador de la lista de miembros pendientes a la lista de miembros activos del clan
      break;

    case Opcode::CLAN_RECHAZAR:
      // Chequear:
      // - El jugador es lider del clan
      // - El jugador a rechazar esta en la lista de miembros pendientes del clan
      // Hacer:
      // - Sacar al jugador de la lista de miembros pendientes del clan
      break;

    case Opcode::CLAN_BAN:
      // Chequear:
      // - El jugador es lider del clan
      // - El jugador a banear esta en la lista de miembros activos del clan
      // Hacer:
      // - Sacar al jugador de la lista de miembros activos del clan y agregarlo a la lista de miembros baneados
      break;

    case Opcode::CLAN_KICK:
      // Chequear:
      // - El jugador es lider del clan
      // - El jugador a kickear esta en la lista de miembros activos del clan
      // Hacer:
      // - Sacar al jugador de la lista de miembros activos del clan (puede volver a pedir unirse)
      break;
  }
  return {};
}

