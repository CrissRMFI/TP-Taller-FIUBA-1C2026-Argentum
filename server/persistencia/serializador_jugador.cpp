#include "serializador_jugador.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#include "../game/jugador.h"

Jugador SerializadorJugador::aJugador(uint16_t sessionId,
                                      const RegistroJugador& reg,
                                      const ConfigJuego& cfg,
                                      const CatalogoItems& catalogo) {
    const std::string nombre(reg.nombre);
    const ClasePersonaje clase = static_cast<ClasePersonaje>(reg.clase);
    const Raza raza = static_cast<Raza>(reg.raza);
    const Posicion posicion{reg.posX, reg.posY, reg.mapaId};

    Jugador jugador(sessionId, nombre, clase, raza, reg.skinCabeza, reg.skinCuerpo, posicion, cfg);

    DatosRestauracion datos;
    datos.skinCabeza = reg.skinCabeza;
    datos.skinCuerpo = reg.skinCuerpo;
    datos.estado = static_cast<Estado>(reg.estado);
    datos.nivel = reg.nivel;
    datos.experiencia = reg.experiencia;
    datos.vidaActual = reg.vidaActual;
    datos.manaActual = reg.manaActual;
    datos.oroMano = reg.oroMano;
    datos.oroExceso = reg.oroExceso;
    datos.oroBanco = reg.oroBanco;
    datos.oroPerdidoPendiente = reg.oroPerdidoPendiente;

    const size_t maxInv = std::size(reg.inventarioSlots);
    datos.slotsInventario.reserve(maxInv);
    for (size_t i = 0; i < maxInv; ++i) {
        datos.slotsInventario.push_back(reg.inventarioSlots[i]);
    }

    datos.equipArma = reg.equipArma;
    datos.equipBaculo = reg.equipBaculo;
    datos.equipDefensa = reg.equipDefensa;
    datos.equipCasco = reg.equipCasco;
    datos.equipEscudo = reg.equipEscudo;

    const size_t maxBanco = std::size(reg.itemsBanco);
    for (size_t i = 0; i < maxBanco; ++i) {
        if (reg.itemsBanco[i] != 0) {
            datos.itemsBanco.push_back(reg.itemsBanco[i]);
        }
    }

    jugador.restaurar(datos);
    jugador.recalcularVestimenta(catalogo);

    return jugador;
}

RegistroJugador SerializadorJugador::aRegistro(const Jugador& jugador) {
    RegistroJugador reg{};

    const std::string nombre = jugador.getNombre();
    const size_t maxNombre = sizeof(reg.nombre) - 1;
    const size_t lenCopiar = std::min(nombre.size(), maxNombre);
    std::memcpy(reg.nombre, nombre.data(), lenCopiar);

    reg.clase = static_cast<uint8_t>(jugador.getClase());
    reg.raza = static_cast<uint8_t>(jugador.getRaza());
    reg.nivel = jugador.getNivel();

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

    reg.skinCabeza = jugador.getCabeza();
    reg.skinCuerpo = jugador.getCuerpo();

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
