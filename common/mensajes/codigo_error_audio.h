#ifndef CODIGO_ERROR_AUDIO_H
#define CODIGO_ERROR_AUDIO_H

#include <cstdint>

enum class CodigoErrorAudio : uint8_t {
    SUBSISTEMA_NO_INICIADO = 0,
    DISPOSITIVO_NO_ABIERTO = 1,
    CONFIG_NO_LEIDA = 2,
    EFECTO_NO_CARGADO = 3,
    MUSICA_NO_CARGADA = 4,
    MUSICA_NO_REPRODUCIDA = 5,
};

#endif
