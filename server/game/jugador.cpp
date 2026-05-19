#include "jugador.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "objeto/catalogo_items.h"

static std::mt19937 rng(std::random_device{}());

Jugador::Jugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion, const ConfigJuego& config)
    : idJugador(id), idClan(0), nombre(nombre),
      nivel(1), experiencia(0),
      oroMano(0), oroExceso(0), oroBanco(0),
      posicion(posicion),
      clase(clase), estado(Estado::Vivo),
      raza(raza), fundadoClan(false), cfg(config)
{
    const StatsRaza& sr = cfg.statsRaza(raza);
    fuerza       = (uint8_t)(sr.fuerza);
    agilidad     = (uint8_t)(sr.agilidad);
    inteligencia = (uint8_t)(sr.inteligencia);
    constitucion = (uint8_t)(sr.constitucion);

    vidaMax    = (uint16_t)(constitucion * cfg.factorVidaClase(clase) * sr.fVida  * nivel);
    manaMax    = (uint16_t)(inteligencia * cfg.factorManaClase(clase) * sr.fMana  * nivel);
    vidaActual = vidaMax;
    manaActual = manaMax;
}

void Jugador::recibir_danio(uint16_t cantidad) {
    if (cfg.invulnerable || !estaVivo()) return;
    if (vidaActual <= cantidad) {
        vidaActual = 0;
        morir();
    } else {
        vidaActual -= cantidad;
    }
}

void Jugador::curar(uint16_t cantidad) {
    if (!estaVivo()) return;
    vidaActual = (uint16_t)(std::min<uint32_t>(vidaActual + cantidad, vidaMax));
}

void Jugador::recuperar_mana(uint16_t cantidad) {
    manaActual = (uint16_t)(std::min<uint32_t>(manaActual + cantidad, manaMax));
}

void Jugador::recuperar(float segundos) {
    
    if (!estaVivo()) return;

    // Recuperación natural de vida: FRazaRecuperacion * segundos
    if (vidaActual < vidaMax) {
        float tasa = cfg.statsRaza(raza).fRecuperacion;
        uint16_t delta = (uint16_t)(tasa * segundos);
        vidaActual = (uint16_t)(std::min<uint32_t>(vidaActual + delta, vidaMax));
    }

    // Recuperación natural de maná (siempre activa)
    if (manaActual < manaMax) {
        float tasa = cfg.statsRaza(raza).fRecuperacion;
        uint16_t delta = (uint16_t)(tasa * segundos);
        manaActual = (uint16_t)(std::min<uint32_t>(manaActual + delta, manaMax));
    }

    // Meditación: recuperación adicional más rápida de maná
    if (estado == Estado::Meditando && manaActual < manaMax) {
        float tasa = cfg.factorMeditacionClase(clase) * inteligencia;
        uint16_t delta = (uint16_t)(tasa * segundos);
        manaActual = (uint16_t)(std::min<uint32_t>(manaActual + delta, manaMax));
        if (manaActual >= manaMax) {
            manaActual = manaMax;
            estado     = Estado::Vivo;
        }
    }

    if (cfg.vidaInfinita) vidaActual = vidaMax;
    if (cfg.manaInfinito) manaActual = manaMax;
}


void Jugador::ganar_experiencia(uint32_t cantidad) {
    if (cfg.expX10) cantidad *= 10;
    experiencia += cantidad;
    uint32_t limite = (uint32_t)(cfg.expLimiteBase * std::pow((float)(nivel), cfg.expLimiteExp));
    if (experiencia >= limite) {
        experiencia -= limite;
        subirNivel();
    }
}

void Jugador::sumar_oro(uint32_t cantidad) {
    uint32_t oroSeguro   = (uint32_t)(100.0f * std::pow((float)(nivel), cfg.oroMaxExp));
    uint32_t limiteExceso = (uint32_t)(oroSeguro * cfg.oroExcesoPct);

    if (oroMano < oroSeguro) {
        uint32_t espacio = oroSeguro - oroMano;
        uint32_t aMano   = std::min(cantidad, espacio);
        oroMano  += aMano;
        cantidad -= aMano;
    }

    if (cantidad > 0 && oroExceso < limiteExceso) {
        uint32_t espacio = limiteExceso - oroExceso;
        oroExceso += std::min(cantidad, espacio);
        // el oro que no cabe se pierde al piso (requiere Mapa para dropearlo)
    }
}

bool Jugador::gastar_oro(uint32_t cantidad) {
    if (oroMano + oroExceso < cantidad) return false;
    if (oroMano >= cantidad) {
        oroMano -= cantidad;
    } else {
        uint32_t restante = cantidad - oroMano;
        oroMano   = 0;
        oroExceso -= restante;
    }
    return true;
}

void Jugador::mover_a(uint16_t x, uint16_t y) {
    posicion.x = x;
    posicion.y = y;
    cancelarMeditacion();
}

void Jugador::cancelarMeditacion() {
    if (estado == Estado::Meditando)
        estado = Estado::Vivo;
}

void Jugador::resucitar(uint16_t x, uint16_t y) {
    estado     = Estado::Vivo;
    posicion.x = x;
    posicion.y = y;
    vidaActual = vidaMax / 2;
    manaActual = 0;
}

void Jugador::meditar() {
    estado = Estado::Meditando;
}

uint16_t Jugador::calcular_danio(const CatalogoItems& catalogo) {
    uint8_t danioMin = 1, danioMax = 1;

    uint16_t idArma   = inventario.getArmaEquipada();
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

    if (danioMax < danioMin) danioMax = danioMin;
    uint16_t base = static_cast<uint16_t>(fuerza) *
                    std::uniform_int_distribution<uint16_t>(danioMin, danioMax)(rng);
    if (es_golpe_critico()) base = static_cast<uint16_t>(base * 2);
    return base;
}


bool Jugador::agregar_item(uint16_t idItem) {
    return inventario.agregarItem(idItem);
}

bool Jugador::eliminar_item(uint16_t idItem) {
    return inventario.eliminarItem(idItem);
}

bool Jugador::equipar_item(uint8_t indice, const CatalogoItems& catalogo) {
    uint16_t idItem = inventario.getIdEnSlot(indice);
    if (idItem == 0) return false;

    const Item* item = catalogo.buscar(idItem);
    if (!item) return false;

    if (item->getTipo() == TipoItem::Defensa) {
        const Defensa* def = static_cast<const Defensa*>(item);
        return inventario.equiparPieza(idItem, def->getSlot());
    }
    return inventario.equiparItem(idItem, item->getTipo());
}

void Jugador::agregar_item_banco(uint16_t idItem) {
    if (!inventario.eliminarItem(idItem)) return;
    idItemsBanco.push_back(idItem);
}

void Jugador::agregar_oro_banco(uint32_t cantidad) {
    if (gastar_oro(cantidad))
        oroBanco += cantidad;
}

bool Jugador::sacar_item_banco(uint16_t idItem) {
    auto it = std::find(idItemsBanco.begin(), idItemsBanco.end(), idItem);
    if (it == idItemsBanco.end()) return false;
    if (!inventario.agregarItem(idItem)) return false;  // inventario lleno
    idItemsBanco.erase(it);
    return true;
}

bool Jugador::sacar_oro_banco(uint32_t cantidad) {
    if (oroBanco < cantidad) return false;
    oroBanco -= cantidad;
    sumar_oro(cantidad);
    return true;
}


void Jugador::asignarClan(uint16_t id) { idClan = id; }

void Jugador::salirClan() {
    idClan       = 0;
    fundadoClan  = false;
}

void Jugador::marcarFundadorClan() { fundadoClan = true; }


bool Jugador::puedeMediatar()  const { 
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

// Getters

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
  return vidaActual; }
uint16_t    Jugador::getVidaMax() const { 
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
  return nivel <= (uint8_t)(cfg.nivelNewbie); 
}
std::string Jugador::getNombre() const { 
  return nombre; 
}
Posicion Jugador::getPosicion() const { 
  return posicion; 
}
Estado  Jugador::getEstado() const { 
  return estado; 
}

std::vector<uint16_t> Jugador::getIdItemsBanco() const { 
  return idItemsBanco; 
}

// Privados 

void Jugador::subirNivel() {
    nivel++;
    const StatsRaza& sr = cfg.statsRaza(raza);
    vidaMax    = (uint16_t)(constitucion * cfg.factorVidaClase(clase) * sr.fVida * nivel);
    manaMax    = (uint16_t)(inteligencia * cfg.factorManaClase(clase) * sr.fMana * nivel);
    vidaActual = vidaMax;
    manaActual = manaMax;
}

void Jugador::morir() {
    estado = Estado::Fantasma;
    if (!es_newbie())
        perder_experiencia(experiencia / 10);
    oroExceso = 0;
}

void Jugador::perder_experiencia(uint32_t cantidad) {
    experiencia = (experiencia >= cantidad) ? experiencia - cantidad : 0;
}

bool Jugador::consumir_mana(uint16_t cantidad) {
    if (manaActual < cantidad) return false;
    manaActual -= cantidad;
    return true;
}

void Jugador::consumir_item(uint16_t idItem) {
    inventario.eliminarItem(idItem);
}

bool Jugador::esquiva_ataque() {
    float r = std::uniform_real_distribution<float>(0.f, 1.f)(rng);
    return std::pow(r, (float)(agilidad)) < cfg.esquivarUmbral;
}

bool Jugador::es_golpe_critico() {
    return std::uniform_int_distribution<int>(0, 99)(rng) < 10;
}
