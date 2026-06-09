#ifndef MENSAJES_ERROR_AUDIO_H
#define MENSAJES_ERROR_AUDIO_H

#include "codigo_error_audio.h"

class MensajesErrorAudio {
public:
    static const char* mensaje(CodigoErrorAudio codigo);

    MensajesErrorAudio() = delete;
};

#endif
