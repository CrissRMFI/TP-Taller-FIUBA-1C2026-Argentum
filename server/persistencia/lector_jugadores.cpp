#include "lector_jugadores.h"

#include <fstream>
#include <ios>

#include "../../common/persistencia/error_persistencia.h"

LectorJugadores::LectorJugadores(const std::string& rutaJugadoresBin,
                                 const IndiceJugadores& indice) :
        rutaJugadores(rutaJugadoresBin), indice(indice) {}

std::optional<RegistroJugador> LectorJugadores::cargar(const std::string& nombre) {
    std::optional<uint64_t> offsetOpt = indice.obtenerOffset(nombre);
    if (!offsetOpt.has_value()) {
        return std::nullopt;
    }

    std::ifstream archivo(rutaJugadores, std::ios::binary);
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, rutaJugadores);
    }

    archivo.seekg(static_cast<std::streamoff>(*offsetOpt));
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::EOF_PREMATURO, rutaJugadores);
    }

    RegistroJugador registro{};
    archivo.read(reinterpret_cast<char*>(&registro), sizeof(registro));
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::EOF_PREMATURO, rutaJugadores);
    }

    return registro;
}
