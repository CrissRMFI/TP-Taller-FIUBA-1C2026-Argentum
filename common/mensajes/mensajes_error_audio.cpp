#include "mensajes_error_audio.h"

const char* MensajesErrorAudio::mensaje(CodigoErrorAudio codigo) {
    switch (codigo) {
        case CodigoErrorAudio::SUBSISTEMA_NO_INICIADO:
            return "No se pudo iniciar el subsistema de audio; el juego corre sin sonido";

        case CodigoErrorAudio::DISPOSITIVO_NO_ABIERTO:
            return "No se pudo abrir el dispositivo de audio; el juego corre sin sonido";

        case CodigoErrorAudio::CONFIG_NO_LEIDA:
            return "No se pudo leer la configuracion de audio; el juego corre sin sonido";

        case CodigoErrorAudio::EFECTO_NO_CARGADO:
            return "No se pudo cargar un efecto de sonido";

        case CodigoErrorAudio::MUSICA_NO_CARGADA:
            return "No se pudo cargar una pista de musica";

        case CodigoErrorAudio::MUSICA_NO_REPRODUCIDA:
            return "No se pudo reproducir la musica";

        default:
            return "Error de audio desconocido";
    }
}
