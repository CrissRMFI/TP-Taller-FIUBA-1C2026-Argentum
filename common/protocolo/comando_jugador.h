#ifndef COMANDO_JUGADOR_H
#define COMANDO_JUGADOR_H

#include <cstdint>
#include <string>
#include <variant>

#include "./opcode.h"

// Comandos sin payload
struct ComandoMeditar     {};
struct ComandoResucitar   {};
struct ComandoTomar       {};
struct ComandoRevisarClan {};
struct ComandoDejarClan   {};

// Comandos con payload
struct ComandoMover { 
  uint8_t direccion;
};

struct ComandoAtacar { 
  uint16_t idObjetivo; 
};

struct ComandoTirar {
  uint8_t indiceItem;
};

struct ComandoEquipar { 
  uint8_t indiceItem; 
};

struct ComandoComprar { 
  uint16_t idItem;
  uint16_t idNPC; 
};

struct ComandoVender { 
  uint8_t indiceItem; 
  uint16_t idNPC; 
};

struct ComandoDepositarItem { 
  uint8_t indiceItem; 
  uint16_t idBanquero; 
};

struct ComandoDepositarOro { 
  uint32_t monto; 
  uint16_t idBanquero; 
};

struct ComandoRetirarItem { 
  uint16_t idItem; 
  uint16_t idBanquero; 
};

struct ComandoRetirarOro { 
  uint32_t monto; 
  uint16_t idBanquero; 
};

struct ComandoListar { 
  uint16_t idNPC; 
};

struct ComandoCurar { 
  uint16_t idSacerdote; 
};

struct ComandoChatGlobal { 
  std::string mensaje; 
};

struct ComandoChatPrivado { 
  std::string nickDestino;
  std::string mensaje; 
};

struct ComandoFundarClan { 
  std::string nombreClan; 
};

struct ComandoUnirseClan { 
  std::string nombreClan; 
};

struct ComandoGestionMiembreClan { 
  std::string nick; 
};

using PayloadComando = std::variant<
    ComandoMeditar,
    ComandoResucitar,
    ComandoTomar,
    ComandoRevisarClan,
    ComandoDejarClan,
    ComandoMover,
    ComandoAtacar,
    ComandoTirar,
    ComandoEquipar,
    ComandoComprar,
    ComandoVender,
    ComandoDepositarItem,
    ComandoDepositarOro,
    ComandoRetirarItem,
    ComandoRetirarOro,
    ComandoListar,
    ComandoCurar,
    ComandoChatGlobal,
    ComandoChatPrivado,
    ComandoFundarClan,
    ComandoUnirseClan,
    ComandoGestionMiembreClan
>;

struct ComandoJugador {
    Opcode opcode;
    PayloadComando payload;
};

#endif
