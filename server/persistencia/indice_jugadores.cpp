#include "indice_jugadores.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <ios>

#include "../../common/persistencia/error_persistencia.h"
#include "../../common/persistencia/formato_jugador.h"

IndiceJugadores::IndiceJugadores(const std::string& rutaIndiceBin)
    : rutaIndice(rutaIndiceBin) {
    cargarDesdeArchivo();
}

void IndiceJugadores::cargarDesdeArchivo() {
    std::ifstream archivo(rutaIndice, std::ios::binary);
    if (!archivo) {
        return;
    }

    archivo.seekg(0, std::ios::end);
    const auto fileSize = archivo.tellg();
    if (fileSize < 0) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, rutaIndice);
    }
    if (static_cast<size_t>(fileSize) % sizeof(IndiceEntrada) != 0) {
        throw ErrorPersistencia(CodigoErrorPersistencia::BYTES_EXTRA, rutaIndice);
    }
    archivo.seekg(0);

    const auto cantEntradas = static_cast<size_t>(fileSize) / sizeof(IndiceEntrada);
    for (size_t i = 0; i < cantEntradas; ++i) {
        IndiceEntrada entrada{};
        archivo.read(reinterpret_cast<char*>(&entrada), sizeof(entrada));
        if (!archivo) {
            throw ErrorPersistencia(CodigoErrorPersistencia::EOF_PREMATURO, rutaIndice);
        }
        std::string nombre(entrada.nombre);
        mapa[nombre] = entrada.offset;
    }
}

std::optional<uint64_t> IndiceJugadores::obtenerOffset(const std::string& nombre) const {
    auto it = mapa.find(nombre);
    if (it == mapa.end()) {
        return std::nullopt;
    }
    return it->second;
}

void IndiceJugadores::agregarEntrada(const std::string& nombre, uint64_t offset) {
    mapa[nombre] = offset;

    IndiceEntrada entrada{};
    const size_t maxNombre = sizeof(entrada.nombre) - 1;
    const size_t lenCopiar = std::min(nombre.size(), maxNombre);
    std::memcpy(entrada.nombre, nombre.data(), lenCopiar);
    entrada.offset = offset;

    std::ofstream archivo(rutaIndice, std::ios::binary | std::ios::app);
    if (!archivo) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, rutaIndice);
    }

    archivo.write(reinterpret_cast<const char*>(&entrada), sizeof(entrada));
    if (!archivo) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::NO_SE_PUEDE_ESCRIBIR, rutaIndice);
    }
}

size_t IndiceJugadores::cantidadEntradas() const {
    return mapa.size();
}
