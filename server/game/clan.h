#ifndef CLAN_H
#define CLAN_H

#include <cstdint>
#include <string>
#include <vector>

enum class EstadoMiembro {
    Pendiente,
    Aceptado,
    Baneado
};

struct MiembroClan {
    uint16_t idJugador;
    EstadoMiembro estado;
};

class Clan {
public:
    Clan(const std::string& nombre, uint16_t idFundador);

    void agregarMiembro(const uint16_t& idJugador);
    void pedirUnirse(const uint16_t& idJugador);
    void eliminarMiembro(const uint16_t& idJugador);
    void banearMiembro(const uint16_t& idJugador);
    bool esMiembro(const uint16_t& idJugador) const;
    std::vector<uint16_t> obtenerMiembros() const;

private:
    uint16_t idClan;
    std::string nombre;
    uint16_t idFundador;
    std::vector<MiembroClan> miembros;
};

#endif
