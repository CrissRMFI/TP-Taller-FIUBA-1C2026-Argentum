#ifndef MODELO_JUEGO_H
#define MODELO_JUEGO_H

#include <cstdint>
#include <vector>

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/protocolo/mensaje_servidor.h"

class ModeloJuego {
public:
    std::vector<MensajeServidor> ejecutarComando(const uint16_t idCliente, const ComandoJugador& comando);
};

#endif
