#ifndef CODIGO_ERROR_ACCION_H
#define CODIGO_ERROR_ACCION_H

#include <cstdint>

enum class CodigoErrorAccion : uint8_t {
    INVENTARIO_LLENO = 0,
    ORO_INSUFICIENTE = 1,
    NIVEL_INSUFICIENTE = 2,
    ACCION_NO_PERMITIDA = 3,
    OBJETIVO_INVALIDO = 4,
    MANA_INSUFICIENTE = 6,
    FUERA_DE_RANGO = 7,        // el objetivo esta demasiado lejos
    ZONA_SEGURA = 8,           // no se puede atacar en/desde una ciudad/pueblo
    CLASE_SIN_MAGIA = 9,       // el guerrero no usa magia
    HECHIZO_YA_CONOCIDO = 10,  // ya aprendiste ese hechizo
    COOLDOWN_ATAQUE = 11,      // todavia no podes volver a atacar
    OBJETIVO_MUERTO = 12,      // el objetivo (o vos) no esta vivo
    USUARIO_YA_CONECTADO = 13, // ya hay una sesion activa con ese usuario/nick
    SIN_POSICION_LIBRE = 14,   // no hay una celda libre donde aparecer
    ID_EN_USO = 15,            // el id de personaje ya esta en uso
    NO_SE_PUDO_CARGAR_PERSONAJE = 16,  // fallo la carga del personaje guardado en disco
    ESTAS_MUERTO = 17,             // la accion requiere estar vivo
    NO_PODES_RESUCITAR = 23,       // no sos fantasma o estas inmovilizado
    ESTAS_INMOVILIZADO = 24,       // no podes hacerlo mientras estas inmovilizado
    ATAQUE_NEWBIE = 25,            // no se puede atacar a personajes nuevos
    DIFERENCIA_NIVEL_EXCESIVA = 26,// la diferencia de nivel es demasiado grande
    HECHIZO_NO_OFENSIVO = 28,      // ese hechizo no sirve para atacar
    HECHIZO_NO_CONOCIDO = 29,      // no conoces ese hechizo
};

#endif
