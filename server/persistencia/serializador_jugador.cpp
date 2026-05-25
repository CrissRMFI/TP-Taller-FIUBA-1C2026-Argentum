#include "serializador_jugador.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <vector>

#include "../game/jugador.h"

RegistroJugador SerializadorJugador::aRegistro(const Jugador& jugador) {
    RegistroJugador reg{};

    const std::string nombre = jugador.getNombre();
    const size_t maxNombre = sizeof(reg.nombre) - 1;
    const size_t lenCopiar = std::min(nombre.size(), maxNombre);
    std::memcpy(reg.nombre, nombre.data(), lenCopiar);

    reg.idClan = jugador.getClan();
    reg.clase = static_cast<uint8_t>(jugador.getClase());
    reg.raza = static_cast<uint8_t>(jugador.getRaza());
    reg.nivel = jugador.getNivel();
    reg.fundadoClan = jugador.fundo_clan() ? 1 : 0;

    Estado estado = jugador.getEstado();
    if (estado == Estado::Meditando) {
        estado = Estado::Vivo;
    }
    if (estado == Estado::Resucitando) {
        estado = Estado::Fantasma;
    }
    reg.estado = static_cast<uint8_t>(estado);

    reg.vidaActual = jugador.getVidaActual();
    reg.manaActual = jugador.getManaActual();
    reg.experiencia = jugador.getExperiencia();

    reg.oroMano = jugador.getOroMano();
    reg.oroExceso = jugador.getOroExceso();
    reg.oroBanco = jugador.getOroBanco();
    reg.oroPerdidoPendiente = jugador.getOroPerdidoPendiente();

    const Posicion pos = jugador.getPosicion();
    reg.mapaId = pos.mapaId;
    reg.posX = pos.x;
    reg.posY = pos.y;

    const std::vector<uint16_t> slots = jugador.getSlotsInventario();
    const size_t maxInv = std::size(reg.inventarioSlots);
    const size_t nInv = std::min(slots.size(), maxInv);
    for (size_t i = 0; i < nInv; ++i) {
        reg.inventarioSlots[i] = slots[i];
    }

    reg.equipArma = jugador.getArmaEquipada();
    reg.equipBaculo = jugador.getBaculoEquipado();
    reg.equipDefensa = jugador.getDefensaEquipada();
    reg.equipCasco = jugador.getCascoEquipado();
    reg.equipEscudo = jugador.getEscudoEquipado();

    const std::vector<uint16_t> banco = jugador.getIdItemsBanco();
    const size_t maxBanco = std::size(reg.itemsBanco);
    const size_t nBanco = std::min(banco.size(), maxBanco);
    for (size_t i = 0; i < nBanco; ++i) {
        reg.itemsBanco[i] = banco[i];
    }

    return reg;
}
