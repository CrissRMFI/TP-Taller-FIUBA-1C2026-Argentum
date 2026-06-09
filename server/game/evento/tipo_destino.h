#ifndef TIPO_DESTINO_H
#define TIPO_DESTINO_H

// Destino de un evento del juego. Vive en la capa de dominio porque la decisión "este evento le corresponde a un solo jugador o a todos" es una decisión del modelo, no del transporte.
enum class TipoDestino {
    UNO,
    TODOS,
    TODOS_EXCEPTO_UNO
};

#endif
