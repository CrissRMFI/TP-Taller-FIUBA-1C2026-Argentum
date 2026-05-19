#include "clan.h"

#include <algorithm>

Clan::Clan(uint16_t idClan, const std::string& nombre, uint16_t idFundador)
    : idClan(idClan), nombre(nombre), idFundador(idFundador) {
    miembros.push_back({ idFundador, EstadoMiembro::Aceptado });
}

void Clan::agregarMiembro(const uint16_t& idJugador) {
    for (auto& m : miembros) {
        if (m.idJugador == idJugador) {
            m.estado = EstadoMiembro::Aceptado;
            return;
        }
    }
    miembros.push_back({ idJugador, EstadoMiembro::Aceptado });
}

void Clan::pedirUnirse(const uint16_t& idJugador) {
    for (auto& m : miembros) {
        if (m.idJugador == idJugador) {
            m.estado = EstadoMiembro::Pendiente;
            return;
        }
    }
    miembros.push_back({ idJugador, EstadoMiembro::Pendiente });
}

void Clan::eliminarMiembro(const uint16_t& idJugador) {
    miembros.erase(
        std::remove_if(miembros.begin(), miembros.end(),
            [&](const MiembroClan& m) { return m.idJugador == idJugador; }),
        miembros.end());
}

void Clan::banearMiembro(const uint16_t& idJugador) {
    for (auto& m : miembros) {
        if (m.idJugador == idJugador) {
            m.estado = EstadoMiembro::Baneado;
            return;
        }
    }
    miembros.push_back({ idJugador, EstadoMiembro::Baneado });
}

bool Clan::esMiembro(const uint16_t& idJugador) const {
    for (const auto& m : miembros)
        if (m.idJugador == idJugador && m.estado == EstadoMiembro::Aceptado)
            return true;
    return false;
}

bool Clan::esFundador(const uint16_t& idJugador) const {
    return idJugador == idFundador;
}

bool Clan::estaBaneado(const uint16_t& idJugador) const {
    for (const auto& m : miembros)
        if (m.idJugador == idJugador && m.estado == EstadoMiembro::Baneado)
            return true;
    return false;
}

bool Clan::estaPendiente(const uint16_t& idJugador) const {
    for (const auto& m : miembros)
        if (m.idJugador == idJugador && m.estado == EstadoMiembro::Pendiente)
            return true;
    return false;
}

uint16_t Clan::getId() const { return idClan; }

size_t Clan::cantidadMiembros() const {
    size_t count = 0;
    for (const auto& m : miembros)
        if (m.estado == EstadoMiembro::Aceptado) count++;
    return count;
}

std::string Clan::getNombre() const { return nombre; }

std::vector<uint16_t> Clan::obtenerMiembros() const {
    std::vector<uint16_t> resultado;
    for (const auto& m : miembros)
        if (m.estado == EstadoMiembro::Aceptado && m.idJugador != idFundador)
            resultado.push_back(m.idJugador);
    return resultado;
}

std::vector<uint16_t> Clan::obtenerPendientes() const {
    std::vector<uint16_t> resultado;
    for (const auto& m : miembros)
        if (m.estado == EstadoMiembro::Pendiente)
            resultado.push_back(m.idJugador);
    return resultado;
}
