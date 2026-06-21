#ifndef COMANDO_JUGADOR_H
#define COMANDO_JUGADOR_H

#include <cstdint>
#include <string>
#include <variant>

#include "./opcode.h"

// Comandos sin payload
struct ComandoMeditar {};
struct ComandoResucitar {};
struct ComandoTomar {};
struct ComandoDetenerMover {};

// Comandos con payload
struct ComandoEmpezarMover {
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

// Ranura de equipo a desequipar
enum class RanuraEquip : uint8_t {
    Arma = 0,
    Baculo = 1,
    Defensa = 2,
    Casco = 3,
    Escudo = 4,
};

struct ComandoDesequipar {
    uint8_t ranura;  // ver RanuraEquip
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

struct ComandoComprarHechizo {
    uint16_t idHechizo;
    uint16_t idSacerdote;
};

struct ComandoLanzarHechizo {
    uint16_t idHechizo;
    uint16_t idObjetivo;
};

struct ComandoChatGlobal {
    std::string mensaje;
};

struct ComandoChatPrivado {
    std::string nickDestino;
    std::string mensaje;
};

// Cheats de prueba (vida/mana infinitos, morir al instante). El cliente los
// dispara por teclas; el efecto lo aplica el servidor sobre el Jugador.
enum class TipoCheat : uint8_t {
    VidaInfinita = 0,
    ManaInfinito = 1,
    MorirAuto = 2,
    DarOro = 3,
};

struct ComandoCheat {
    uint8_t tipo;
};

using PayloadComando =
        std::variant<ComandoMeditar, ComandoResucitar, ComandoTomar, ComandoEmpezarMover,
                     ComandoDetenerMover, ComandoAtacar, ComandoTirar, ComandoEquipar,
                     ComandoDesequipar, ComandoComprar, ComandoVender, ComandoDepositarItem,
                     ComandoDepositarOro, ComandoRetirarItem, ComandoRetirarOro, ComandoListar,
                     ComandoCurar, ComandoComprarHechizo, ComandoLanzarHechizo, ComandoChatGlobal,
                     ComandoChatPrivado, ComandoCheat>;

struct ComandoJugador {
    Opcode opcode;
    PayloadComando payload;
};

#endif
