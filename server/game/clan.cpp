#include "clan.h"

#include <algorithm>

Clan::Clan(uint16_t idClan, const std::string& nombre, const std::string& nickFundador)
    : idClan(idClan), nombre(nombre), nickFundador(nickFundador) {
    miembros.push_back({ nickFundador, EstadoMiembro::Aceptado });
}

void Clan::agregarMiembro(const std::string& nickJugador) {
    for (auto& miembro: miembros) {
        if (miembro.nickJugador == nickJugador) {
            miembro.estado = EstadoMiembro::Aceptado;
            return;
        }
    }

    miembros.push_back({ nickJugador, EstadoMiembro::Aceptado });
}

void Clan::pedirUnirse(const std::string& nickJugador) {
    for (auto& miembro: miembros) {
        if (miembro.nickJugador == nickJugador) {
            miembro.estado = EstadoMiembro::Pendiente;
            return;
        }
    }

    miembros.push_back({ nickJugador, EstadoMiembro::Pendiente });
}

void Clan::eliminarMiembro(const std::string& nickJugador) {
    miembros.erase(
            std::remove_if(
                    miembros.begin(),
                    miembros.end(),
                    [&](const MiembroClan& miembro) {
                        return miembro.nickJugador == nickJugador;
                    }),
            miembros.end());
}

void Clan::banearMiembro(const std::string& nickJugador) {
    for (auto& miembro: miembros) {
        if (miembro.nickJugador == nickJugador) {
            miembro.estado = EstadoMiembro::Baneado;
            return;
        }
    }

    miembros.push_back({ nickJugador, EstadoMiembro::Baneado });
}

bool Clan::esMiembro(const std::string& nickJugador) const {
    for (const auto& miembro: miembros) {
        if (miembro.nickJugador == nickJugador && miembro.estado == EstadoMiembro::Aceptado) {
            return true;
        }
    }

    return false;
}

bool Clan::esFundador(const std::string& nickJugador) const {
    return nickJugador == nickFundador;
}

bool Clan::estaBaneado(const std::string& nickJugador) const {
    for (const auto& miembro: miembros) {
        if (miembro.nickJugador == nickJugador && miembro.estado == EstadoMiembro::Baneado) {
            return true;
        }
    }

    return false;
}

bool Clan::estaPendiente(const std::string& nickJugador) const {
    for (const auto& miembro: miembros) {
        if (miembro.nickJugador == nickJugador && miembro.estado == EstadoMiembro::Pendiente) {
            return true;
        }
    }

    return false;
}

uint16_t Clan::getId() const {
    return idClan;
}

size_t Clan::cantidadMiembros() const {
    size_t cantidad = 0;

    for (const auto& miembro: miembros) {
        if (miembro.estado == EstadoMiembro::Aceptado) {
            cantidad++;
        }
    }

    return cantidad;
}

std::string Clan::getNombre() const {
    return nombre;
}

std::vector<std::string> Clan::obtenerMiembros() const {
    std::vector<std::string> resultado;

    for (const auto& miembro: miembros) {
        if (miembro.estado == EstadoMiembro::Aceptado &&
            miembro.nickJugador != nickFundador) {
            resultado.push_back(miembro.nickJugador);
        }
    }

    return resultado;
}

std::vector<std::string> Clan::obtenerPendientes() const {
    std::vector<std::string> resultado;

    for (const auto& miembro: miembros) {
        if (miembro.estado == EstadoMiembro::Pendiente) {
            resultado.push_back(miembro.nickJugador);
        }
    }

    return resultado;
}