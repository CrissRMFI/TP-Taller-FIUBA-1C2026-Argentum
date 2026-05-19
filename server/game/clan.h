#ifndef CLAN_H
#define CLAN_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

enum class EstadoMiembro {
    Pendiente,
    Aceptado,
    Baneado
};

struct MiembroClan {
    std::string nickJugador;
    EstadoMiembro estado;
};

class Clan {
public:
    Clan(uint16_t idClan, const std::string& nombre, const std::string& nickFundador);

    void agregarMiembro(const std::string& nickJugador);
    void pedirUnirse(const std::string& nickJugador);
    void eliminarMiembro(const std::string& nickJugador);
    void banearMiembro(const std::string& nickJugador);

    bool esMiembro(const std::string& nickJugador) const;
    bool esFundador(const std::string& nickJugador) const;
    bool estaBaneado(const std::string& nickJugador) const;
    bool estaPendiente(const std::string& nickJugador) const;

    uint16_t getId() const;
    size_t cantidadMiembros() const;
    std::string getNombre() const;
    std::vector<std::string> obtenerMiembros() const;
    std::vector<std::string> obtenerPendientes() const;

private:
    uint16_t idClan;
    std::string nombre;
    std::string nickFundador;
    std::vector<MiembroClan> miembros;
};

#endif
