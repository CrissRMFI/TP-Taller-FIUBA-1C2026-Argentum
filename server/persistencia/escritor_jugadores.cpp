#include "escritor_jugadores.h"

#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <string>

#include "../../common/persistencia/error_persistencia.h"

EscritorJugadores::EscritorJugadores(const std::string& rutaJugadoresBin, IndiceJugadores& indice) :
        rutaJugadores(rutaJugadoresBin), indice(indice) {}

void EscritorJugadores::guardar(const RegistroJugador& registro) {
    const std::string nombre(registro.nombre);
    std::optional<uint64_t> offsetExistente = indice.obtenerOffset(nombre);

    if (offsetExistente.has_value()) {
        escribirEnOffset(registro, *offsetExistente);
    } else {
        const uint64_t offsetNuevo = apenderRegistro(registro);
        indice.agregarEntrada(nombre, offsetNuevo);
    }
}

void EscritorJugadores::escribirEnOffset(const RegistroJugador& registro, uint64_t offset) {
    std::fstream archivo(rutaJugadores, std::ios::binary | std::ios::in | std::ios::out);
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, rutaJugadores);
    }

    archivo.seekp(static_cast<std::streamoff>(offset));
    archivo.write(reinterpret_cast<const char*>(&registro), sizeof(registro));
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::NO_SE_PUEDE_ESCRIBIR, rutaJugadores);
    }
}

uint64_t EscritorJugadores::apenderRegistro(const RegistroJugador& registro) {
    uint64_t offset = 0;
    if (std::filesystem::exists(rutaJugadores)) {
        offset = std::filesystem::file_size(rutaJugadores);
    }

    std::ofstream archivo(rutaJugadores, std::ios::binary | std::ios::app);
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, rutaJugadores);
    }

    archivo.write(reinterpret_cast<const char*>(&registro), sizeof(registro));
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::NO_SE_PUEDE_ESCRIBIR, rutaJugadores);
    }

    return offset;
}
