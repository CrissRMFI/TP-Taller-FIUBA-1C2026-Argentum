#include "jugador.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "objeto/catalogo_items.h"
#include "reglas/reglas_juego.h"

static std::mt19937 rng(std::random_device{}());

static void aplicarRecuperacion(float& pendiente,
                                uint16_t& actual,
                                uint16_t maximo,
                                float delta) {
    if (actual >= maximo) {
        pendiente = 0.0f;
        return;
    }

    pendiente += delta;
    uint32_t puntos = static_cast<uint32_t>(std::floor(pendiente));
    if (puntos == 0) {
        return;
    }

    pendiente -= static_cast<float>(puntos);
    actual = static_cast<uint16_t>(std::min<uint32_t>(actual + puntos, maximo));

    if (actual >= maximo) {
        pendiente = 0.0f;
    }
}

Jugador::Jugador(uint16_t id,
                 const std::string& nombre,
                 ClasePersonaje clase,
                 Raza raza,
                 Posicion posicion,
                 const ConfigJuego& config) :
        idJugador(id),
        idClan(0),
        nombre(nombre),
        nivel(1),
        experiencia(0),
        vidaActual(0),
        vidaMax(0),
        manaActual(0),
        manaMax(0),
        recuperacionVidaPendiente(0.0f),
        recuperacionManaPendiente(0.0f),
        meditacionManaPendiente(0.0f),
        oroMano(0),
        oroExceso(0),
        oroBanco(0),
        fuerza(0),
        agilidad(0),
        inteligencia(0),
        constitucion(0),
        posicion(posicion),
        cfg(config),
        inventario(config.inventarioMaxItems),
        idItemsBanco(),
        clase(clase),
        estado(Estado::Vivo),
        raza(raza),
        fundadoClan(false) {
    const StatsRaza& sr = cfg.statsRaza(raza);

    fuerza = static_cast<uint8_t>(sr.fuerza);
    agilidad = static_cast<uint8_t>(sr.agilidad);
    inteligencia = static_cast<uint8_t>(sr.inteligencia);
    constitucion = static_cast<uint8_t>(sr.constitucion);

    vidaMax = ReglasJuego::calcularVidaMaxima(
            cfg, raza, clase, nivel, constitucion);

    manaMax = ReglasJuego::calcularManaMaximo(
            cfg, raza, clase, nivel, inteligencia);

    vidaActual = vidaMax;
    manaActual = manaMax;
}

void Jugador::recibir_danio(uint16_t cantidad) {
    if (cfg.invulnerable || !estaVivo()) {
        return;
    }

    if (vidaActual <= cantidad) {
        vidaActual = 0;
        morir();
    } else {
        vidaActual -= cantidad;
    }
}

void Jugador::curar(uint16_t cantidad) {
    if (!estaVivo()) {
        return;
    }

    vidaActual = static_cast<uint16_t>(
            std::min<uint32_t>(vidaActual + cantidad, vidaMax));
}

void Jugador::recuperar_mana(uint16_t cantidad) {
    manaActual = static_cast<uint16_t>(
            std::min<uint32_t>(manaActual + cantidad, manaMax));
}

void Jugador::recuperar(float segundos) {
    if (!estaVivo()) {
        return;
    }

    if (vidaActual < vidaMax) {
        aplicarRecuperacion(
                recuperacionVidaPendiente,
                vidaActual,
                vidaMax,
                ReglasJuego::calcularRecuperacionNatural(cfg, raza, segundos));
    } else {
        recuperacionVidaPendiente = 0.0f;
    }

    if (manaActual < manaMax) {
        aplicarRecuperacion(
                recuperacionManaPendiente,
                manaActual,
                manaMax,
                ReglasJuego::calcularRecuperacionNatural(cfg, raza, segundos));
    } else {
        recuperacionManaPendiente = 0.0f;
    }

    if (estado == Estado::Meditando && manaActual < manaMax) {
        aplicarRecuperacion(
                meditacionManaPendiente,
                manaActual,
                manaMax,
                ReglasJuego::calcularRecuperacionMeditacion(
                        cfg, clase, inteligencia, segundos));

        if (manaActual >= manaMax) {
            manaActual = manaMax;
            estado = Estado::Vivo;
        }
    } else if (estado != Estado::Meditando) {
        meditacionManaPendiente = 0.0f;
    }

    if (estado == Estado::Meditando && manaActual >= manaMax) {
        manaActual = manaMax;
        estado = Estado::Vivo;
        meditacionManaPendiente = 0.0f;
    }

    if (cfg.vidaInfinita) {
        vidaActual = vidaMax;
    }

    if (cfg.manaInfinito) {
        manaActual = manaMax;
    }
}

void Jugador::ganar_experiencia(uint32_t cantidad) {
    if (cfg.expX10) {
        cantidad *= 10;
    }

    experiencia += cantidad;

    uint32_t limite = ReglasJuego::calcularLimiteExperiencia(cfg, nivel);

    if (experiencia >= limite) {
        experiencia -= limite;
        subirNivel();
    }
}

void Jugador::sumar_oro(uint32_t cantidad) {
    uint32_t totalActual = oroMano + oroExceso;
    uint32_t maximoTotal = ReglasJuego::calcularOroMaximoTotal(cfg, nivel);

    uint32_t espacioTotal = 0;
    if (totalActual < maximoTotal) {
        espacioTotal = maximoTotal - totalActual;
    }

    uint32_t cantidadAceptada = std::min(cantidad, espacioTotal);

    oroMano += cantidadAceptada;
    normalizarOro();

    // Si cantidad > cantidadAceptada, ese oro no entra en el máximo permitido.
    // Cuando exista mapa, Juego debería decidir si se dropea al suelo.
}

bool Jugador::gastar_oro(uint32_t cantidad) {
    if (oroMano + oroExceso < cantidad) {
        return false;
    }

    uint32_t total = oroMano + oroExceso - cantidad;

    oroMano = total;
    oroExceso = 0;
    normalizarOro();

    return true;
}

void Jugador::mover_a(uint16_t x, uint16_t y) {
    posicion.x = x;
    posicion.y = y;
    cancelarMeditacion();
}

void Jugador::resucitar(uint16_t x, uint16_t y) {
    estado = Estado::Vivo;
    posicion.x = x;
    posicion.y = y;
    vidaActual = vidaMax / 2;
    manaActual = 0;
}

void Jugador::meditar() {
    estado = Estado::Meditando;
}

void Jugador::cancelarMeditacion() {
    if (estado == Estado::Meditando) {
        estado = Estado::Vivo;
    }
}

uint16_t Jugador::calcular_danio(const CatalogoItems& catalogo) {
    uint8_t danioMin = 1;
    uint8_t danioMax = 1;

    uint16_t idArma = inventario.getArmaEquipada();
    uint16_t idBaculo = inventario.getBaculoEquipado();

    if (idArma != 0) {
        if (const Arma* arma = catalogo.comoArma(idArma)) {
            danioMin = arma->getDanioMin();
            danioMax = arma->getDanioMax();
        }
    } else if (idBaculo != 0) {
        if (const Baculo* baculo = catalogo.comoBaculo(idBaculo)) {
            danioMin = baculo->getDanioMin();
            danioMax = baculo->getDanioMax();
        }
    }

    if (danioMax < danioMin) {
        danioMax = danioMin;
    }

    uint16_t danioBase = std::uniform_int_distribution<uint16_t>(
            danioMin, danioMax)(rng);

    uint16_t danio = static_cast<uint16_t>(fuerza) * danioBase;

    if (es_golpe_critico()) {
        danio = static_cast<uint16_t>(danio * 2);
    }

    return danio;
}

bool Jugador::agregar_item(uint16_t idItem) {
    return inventario.agregarItem(idItem);
}

bool Jugador::eliminar_item(uint16_t idItem) {
    return inventario.eliminarItem(idItem);
}

bool Jugador::equipar_item(uint8_t indice, const CatalogoItems& catalogo) {
    uint16_t idItem = inventario.getIdEnSlot(indice);

    if (idItem == 0) {
        return false;
    }

    const Item* item = catalogo.buscar(idItem);
    if (item == nullptr) {
        return false;
    }

    if (item->getTipo() == TipoItem::Pocion) {
        if (!estaVivo()) {
            return false;
        }

        const Pocion* pocion = catalogo.comoPocion(idItem);
        if (pocion == nullptr) {
            return false;
        }

        if (pocion->getTipoPocion() == TipoPocion::Vida) {
            curar(pocion->getCantidad());
        } else {
            recuperar_mana(pocion->getCantidad());
        }

        inventario.eliminarSlot(indice);
        return true;
    }

    if (item->getTipo() == TipoItem::Defensa) {
        const Defensa* defensa = catalogo.comoDefensa(idItem);
        if (defensa == nullptr) {
            return false;
        }

        return inventario.equiparPiezaSlot(indice, defensa->getSlot());
    }

    return inventario.equiparSlot(indice, item->getTipo());
}

bool Jugador::agregar_item_banco(uint8_t indice) {
    uint16_t idItem = inventario.quitarDeSlot(indice);
    if (idItem == 0) {
        return false;
    }

    idItemsBanco.push_back(idItem);
    return true;
}

bool Jugador::agregar_oro_banco(uint32_t cantidad) {
    if (!gastar_oro(cantidad)) {
        return false;
    }

    oroBanco += cantidad;
    return true;
}

bool Jugador::sacar_item_banco(uint16_t idItem) {
    auto it = std::find(idItemsBanco.begin(), idItemsBanco.end(), idItem);

    if (it == idItemsBanco.end()) {
        return false;
    }

    if (!inventario.agregarItem(idItem)) {
        return false;
    }

    idItemsBanco.erase(it);
    return true;
}

bool Jugador::sacar_oro_banco(uint32_t cantidad) {
    if (oroBanco < cantidad) {
        return false;
    }

    uint32_t maximoEnMano = ReglasJuego::calcularOroMaximoTotal(cfg, nivel);
    if (getOro() + cantidad > maximoEnMano) {
        return false;
    }

    oroBanco -= cantidad;
    sumar_oro(cantidad);
    return true;
}

void Jugador::asignarClan(uint16_t id) {
    idClan = id;
}

void Jugador::salirClan() {
    idClan = 0;
    fundadoClan = false;
}

void Jugador::marcarFundadorClan() {
    fundadoClan = true;
}

bool Jugador::puedeMeditar() const {
    return manaMax > 0;
}

bool Jugador::puedeUsarMagia() const {
    return manaMax > 0;
}

bool Jugador::estaVivo() const {
    return estado == Estado::Vivo || estado == Estado::Meditando;
}

bool Jugador::esFantasma() const {
    return estado == Estado::Fantasma || estado == Estado::Resucitando;
}

bool Jugador::enMeditacion() const {
    return estado == Estado::Meditando;
}

bool Jugador::tieneClan() const {
    return idClan != 0;
}

uint16_t Jugador::getId() const {
    return idJugador;
}

uint8_t Jugador::getNivel() const {
    return nivel;
}

uint32_t Jugador::getExperiencia() const {
    return experiencia;
}

uint16_t Jugador::getVidaActual() const {
    return vidaActual;
}

uint16_t Jugador::getVidaMax() const {
    return vidaMax;
}

uint16_t Jugador::getManaActual() const {
    return manaActual;
}

uint16_t Jugador::getManaMax() const {
    return manaMax;
}

uint32_t Jugador::getOro() const {
    return oroMano + oroExceso;
}

uint32_t Jugador::getOroBanco() const {
    return oroBanco;
}

uint16_t Jugador::getClan() const {
    return idClan;
}

bool Jugador::fundo_clan() const {
    return fundadoClan;
}

bool Jugador::es_newbie() const {
    return nivel <= static_cast<uint8_t>(cfg.nivelNewbie);
}

std::string Jugador::getNombre() const {
    return nombre;
}

Posicion Jugador::getPosicion() const {
    return posicion;
}

Estado Jugador::getEstado() const {
    return estado;
}

std::vector<uint16_t> Jugador::getSlotsInventario() const {
    return inventario.getSlots();
}

uint16_t Jugador::getArmaEquipada() const {
    return inventario.getArmaEquipada();
}

uint16_t Jugador::getBaculoEquipado() const {
    return inventario.getBaculoEquipado();
}

uint16_t Jugador::getDefensaEquipada() const {
    return inventario.getDefensaEquipada();
}

uint16_t Jugador::getCascoEquipado() const {
    return inventario.getCascoEquipado();
}

uint16_t Jugador::getEscudoEquipado() const {
    return inventario.getEscudoEquipado();
}

std::vector<uint16_t> Jugador::getIdItemsBanco() const {
    return idItemsBanco;
}

void Jugador::subirNivel() {
    nivel++;

    vidaMax = ReglasJuego::calcularVidaMaxima(
            cfg, raza, clase, nivel, constitucion);

    manaMax = ReglasJuego::calcularManaMaximo(
            cfg, raza, clase, nivel, inteligencia);

    vidaActual = vidaMax;
    manaActual = manaMax;
}

void Jugador::morir() {
    estado = Estado::Fantasma;

    if (!es_newbie()) {
        perder_experiencia(experiencia / 10);
    }

    oroExceso = 0;
}

void Jugador::perder_experiencia(uint32_t cantidad) {
    experiencia = (experiencia >= cantidad) ? experiencia - cantidad : 0;
}

bool Jugador::consumir_mana(uint16_t cantidad) {
    if (manaActual < cantidad) {
        return false;
    }

    manaActual -= cantidad;
    return true;
}

void Jugador::consumir_item(uint16_t idItem) {
    inventario.eliminarItem(idItem);
}

void Jugador::normalizarOro() {
    uint32_t total = oroMano + oroExceso;
    uint32_t oroSeguro = ReglasJuego::calcularOroSeguro(cfg, nivel);

    oroMano = std::min(total, oroSeguro);
    oroExceso = total - oroMano;
}

bool Jugador::esquiva_ataque() {
    float r = std::uniform_real_distribution<float>(0.f, 1.f)(rng);
    return std::pow(r, static_cast<float>(agilidad)) < cfg.esquivarUmbral;
}

bool Jugador::es_golpe_critico() {
    return std::uniform_int_distribution<int>(0, 99)(rng) < 10;
}

void Jugador::actualizarId(uint16_t nuevoId) {
    idJugador = nuevoId;
}