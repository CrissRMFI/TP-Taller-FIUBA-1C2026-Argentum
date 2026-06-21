#ifndef COMMON_PERSISTENCIA_FORMATO_JUGADOR_H
#define COMMON_PERSISTENCIA_FORMATO_JUGADOR_H

#include <cstdint>
/*
Que no incluimos ?
- idJugador: El servidor le asigna un nuevo ID en la reconección
- fuerza, agilidad, inteligencia, constitucion: se pueden recalcular a partir de la raza y el nivel
- vidaMax, manaMax: se pueden recalcular a partir de la raza, clase, nivel y
constitución/inteligencia
- recuperacionVidaPendiente, recuperaciondManaPendiente, meditacionManaPendiente: no es necesario
guardarlos, se pueden recalcular a partir de la experiencia y el tiempo que ha pasado desde la
última vez que se guardó el jugador
*/

struct RegistroJugador {
    char nombre[32];
    uint16_t _reservado0;  // libre (mantiene tamano/offsets del formato)
    uint8_t clase;
    uint8_t raza;
    uint8_t nivel;
    uint8_t _reservado1;  // libre (mantiene tamano/offsets del formato)
    uint8_t estado;
    uint8_t _pad1;  // relleno con esto para no depender de padding automático del compilador

    uint16_t skinCabeza;
    uint16_t skinCuerpo;
    uint16_t vidaActual;
    uint16_t manaActual;
    uint32_t experiencia;
    uint32_t oroMano;
    uint32_t oroExceso;
    uint32_t oroBanco;
    uint32_t oroPerdidoPendiente;

    uint16_t mapaId;
    uint16_t posX;
    uint16_t posY;
    uint16_t _pad2;  // relleno con esto para no depender de padding automático del compilador

    uint16_t inventarioSlots[20];

    uint16_t equipArma;
    uint16_t equipBaculo;
    uint16_t equipDefensa;
    uint16_t equipCasco;
    uint16_t equipEscudo;
    uint16_t _pad3;  // relleno con esto para no depender de padding automático del compilador

    uint16_t itemsBanco[64];
};

static_assert(sizeof(RegistroJugador) == 256);

struct IndiceEntrada {
    char nombre[32];
    uint64_t offset;
};

static_assert(sizeof(IndiceEntrada) == 40);

#endif
