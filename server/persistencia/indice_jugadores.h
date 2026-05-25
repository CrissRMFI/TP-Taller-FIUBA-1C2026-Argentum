#ifndef SERVER_PERSISTENCIA_INDICE_JUGADORES_H
#define SERVER_PERSISTENCIA_INDICE_JUGADORES_H

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

class IndiceJugadores {
public:
    explicit IndiceJugadores(const std::string& rutaIndiceBin);

    std::optional<uint64_t> obtenerOffset(const std::string& nombre) const;

    void agregarEntrada(const std::string& nombre, uint64_t offset);

    size_t cantidadEntradas() const;

private:
    std::string rutaIndice;
    std::unordered_map<std::string, uint64_t> mapa;

    void cargarDesdeArchivo();
};

#endif
