#ifndef SERVER_PERSISTENCIA_ESCRITOR_JUGADORES_H
#define SERVER_PERSISTENCIA_ESCRITOR_JUGADORES_H

#include <cstdint>
#include <string>

#include "../../common/persistencia/formato_jugador.h"
#include "indice_jugadores.h"

class EscritorJugadores {
public:
    EscritorJugadores(const std::string& rutaJugadoresBin, IndiceJugadores& indice);

    void guardar(const RegistroJugador& registro);

private:
    std::string rutaJugadores;
    IndiceJugadores& indice;

    void escribirEnOffset(const RegistroJugador& registro, uint64_t offset);
    uint64_t apenderRegistro(const RegistroJugador& registro);
};

#endif
