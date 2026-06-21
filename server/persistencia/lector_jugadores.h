#ifndef SERVER_PERSISTENCIA_LECTOR_JUGADORES_H
#define SERVER_PERSISTENCIA_LECTOR_JUGADORES_H

#include <optional>
#include <string>

#include "../../common/persistencia/formato_jugador.h"
#include "indice_jugadores.h"

class LectorJugadores {
public:
    LectorJugadores(const std::string& rutaJugadoresBin, const IndiceJugadores& indice);

    std::optional<RegistroJugador> cargar(const std::string& nombre);

private:
    std::string rutaJugadores;
    const IndiceJugadores& indice;
};

#endif
