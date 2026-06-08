#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>

enum class Opcode : uint8_t {

    // Cliente → Servidor
    EMPEZAR_MOVER   = 1,
    ATACAR          = 2,
    MEDITAR         = 3,
    RESUCITAR       = 4,
    CURAR           = 5,
    TOMAR           = 6,
    TIRAR           = 7,
    EQUIPAR         = 8,
    COMPRAR         = 9,
    VENDER          = 10,
    DEPOSITAR_ITEM  = 11,
    DEPOSITAR_ORO   = 12,
    RETIRAR_ITEM    = 13,
    RETIRAR_ORO     = 14,
    LISTAR          = 15,
    CHAT_GLOBAL     = 16,
    CHAT_PRIVADO    = 17,
    FUNDAR_CLAN     = 18,
    UNIRSE_CLAN     = 19,
    REVISAR_CLAN    = 20,
    CLAN_ACEPTAR    = 21,
    CLAN_RECHAZAR   = 22,
    CLAN_BAN        = 23,
    CLAN_KICK       = 24,
    DEJAR_CLAN      = 25,

    // Servidor → Cliente
    ESTADO_PERSONAJE        = 26,
    POSICION_ENTIDAD        = 27,
    ENTIDAD_DESAPARECIO     = 28,
    DANIO_RECIBIDO          = 29,
    DANIO_PRODUCIDO         = 30,
    ESQUIVE                 = 31,
    MUERTE_ENTIDAD          = 32,
    ITEM_EN_SUELO           = 33,
    ITEM_DESAPARECIO_SUELO  = 34,
    ACTUALIZAR_INVENTARIO   = 35,
    ACTUALIZAR_EQUIPAMIENTO = 36,
    MENSAJE_CHAT            = 37,
    MENSAJE_CLAN            = 38,
    RESUCITADO              = 39,
    LISTA_ITEMS             = 40,
    ERROR_ACCION            = 41,
    ORO_EN_SUELO            = 42,
    ORO_DESAPARECIO_SUELO   = 43,
    ESTADO_USUARIO          = 44,

    // Cliente → Servidor (cheats de prueba)
    CHEAT                   = 45,

    // Cliente → Servidor (movimiento: empezar/detener)
    DETENER_MOVER           = 46,

    // Cliente → Servidor (usar item: pociones)
    USAR                    = 47,

    // Servidor → Cliente (contenido del banco: items + oro)
    CONTENIDO_BANCO         = 48,

    // Cliente → Servidor (comprar un hechizo al sacerdote)
    COMPRAR_HECHIZO         = 49,

    // Servidor → Cliente (lista de hechizos conocidos del jugador)
    LISTA_HECHIZOS          = 50,

    // Cliente → Servidor (lanzar un hechizo sobre un objetivo)
    LANZAR_HECHIZO          = 51,

    // Servidor → Cliente (FX visual de un hechizo lanzado, para todos los que lo ven)
    FX_HECHIZO              = 52,

    // Servidor → Cliente (proyectil que viaja del atacante al objetivo, ataque a distancia)
    PROYECTIL               = 53
};

#endif
