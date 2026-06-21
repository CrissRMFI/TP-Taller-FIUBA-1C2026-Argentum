#include "reglas_juego.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include "../modelo/clase_personaje.h"

uint16_t ReglasJuego::calcularVidaMaxima(const ConfigJuego& cfg, Raza raza, ClasePersonaje clase, uint16_t nivel, uint16_t constitucion) {
    float valor = constitucion * cfg.factorVidaClase(clase) * cfg.statsRaza(raza).fVida * nivel;

    return static_cast<uint16_t>(std::max(1.0f, valor));
}

uint16_t ReglasJuego::calcularManaMaximo(const ConfigJuego& cfg, Raza raza, ClasePersonaje clase, uint16_t nivel, uint16_t inteligencia) {
  if (clase == ClasePersonaje::GUERRERO) {
    return 0;
  }
  
  float valor = inteligencia * cfg.factorManaClase(clase) * cfg.statsRaza(raza).fMana * nivel;
  
  return static_cast<uint16_t>(std::max(0.0f, valor));
}

uint32_t ReglasJuego::calcularLimiteExperiencia(const ConfigJuego& cfg, uint16_t nivel) {
    float valor = cfg.expLimiteBase * std::pow(static_cast<float>(nivel), cfg.expLimiteExp);

    return static_cast<uint32_t>(std::max(1.0f, valor));
}

uint32_t ReglasJuego::calcularOroSeguro(const ConfigJuego& cfg, uint16_t nivel) {
    float valor = cfg.oroMaxBase * std::pow(static_cast<float>(nivel), cfg.oroMaxExp);

    return static_cast<uint32_t>(std::max(0.0f, valor));
}       

uint32_t ReglasJuego::calcularOroMaximoTotal(const ConfigJuego& cfg, uint16_t nivel) {
    uint32_t oroSeguro = calcularOroSeguro(cfg, nivel);
    float valor = static_cast<float>(oroSeguro) * (1.0f + cfg.oroExcesoPct);

    return static_cast<uint32_t>(std::max<float>( static_cast<float>(oroSeguro), valor));
}

bool ReglasJuego::esGolpeCritico(const ConfigJuego& cfg, float valorAleatorio) {
    float probabilidad = std::clamp(cfg.probabilidadCritico, 0.0f, 1.0f);
    return valorAleatorio < probabilidad;
}

uint32_t ReglasJuego::calcularPerdidaExperienciaMuerte( const ConfigJuego& cfg, uint32_t experienciaActual) {
    float porcentaje = std::clamp(cfg.expPerdidaMuertePct, 0.0f, 1.0f);
    float perdida = static_cast<float>(experienciaActual) * porcentaje;

    return static_cast<uint32_t>(std::max(0.0f, perdida));
}

float ReglasJuego::calcularRecuperacionNatural(const ConfigJuego& cfg, Raza raza, float segundos) {
    float valor = cfg.statsRaza(raza).fRecuperacion * segundos;

    return std::max(0.0f, valor);
}

uint32_t ReglasJuego::calcularExperienciaImpacto(const ConfigJuego& cfg, uint16_t danioAplicado, uint8_t nivelAtacante, uint8_t nivelObjetivo) {
    const int diff = static_cast<int>(nivelObjetivo) - static_cast<int>(nivelAtacante) + cfg.expBonusNivel;

    if (diff <= 0 || danioAplicado == 0) {
        return 0;
    }

    return static_cast<uint32_t>(danioAplicado) * static_cast<uint32_t>(diff);
}

uint32_t ReglasJuego::calcularExperienciaKill(const ConfigJuego& cfg, uint16_t vidaMaxObjetivo, uint8_t nivelAtacante, uint8_t nivelObjetivo, float valorAleatorio) {

    const int diff = static_cast<int>(nivelObjetivo) - static_cast<int>(nivelAtacante) + cfg.expBonusNivel;

    if (diff <= 0 || vidaMaxObjetivo == 0) {
        return 0;
    }

    const float r = std::clamp(valorAleatorio, 0.0f, 1.0f) * cfg.expKillMax;
    const float xp = r * static_cast<float>(vidaMaxObjetivo) * static_cast<float>(diff);

    return static_cast<uint32_t>(std::max(0.0f, xp));
}

bool ReglasJuego::esquivaAtaque(const ConfigJuego& cfg, uint8_t agilidad, float valorAleatorio) {
    
    const float valorAleatorioNormalizado = std::clamp(valorAleatorio, 0.0f, 1.0f);
    const float probabilidadDeEsquive = std::pow(valorAleatorioNormalizado, static_cast<float>(agilidad));
    return probabilidadDeEsquive < cfg.esquivarUmbral;
}

uint32_t ReglasJuego::calcularDropOroNpc(const ConfigJuego& cfg, uint16_t vidaMaxNpc, float valorAleatorio) {
    if (vidaMaxNpc == 0) {
        return 0;
    }

    const float factor = std::clamp(valorAleatorio, 0.0f, 1.0f) * cfg.oroDropNpcMax;
    const float oro = factor * static_cast<float>(vidaMaxNpc);

    return static_cast<uint32_t>(std::max(0.0f, oro));
}

uint16_t ReglasJuego::aplicarMultiplicadorCombate(uint16_t valor, float multiplicador) {
    const float multiplicadorSeguro = std::max(0.0f, multiplicador);
    const float resultado = static_cast<float>(valor) * multiplicadorSeguro;

    return static_cast<uint16_t>(std::min<float>( static_cast<float> (std::numeric_limits<uint16_t>::max()), resultado));
}

float ReglasJuego::calcularRecuperacionMeditacion(const ConfigJuego& cfg, ClasePersonaje clase, uint16_t inteligencia, float segundos) {
    if (clase == ClasePersonaje::GUERRERO) {
        return 0.0f;
    }

    float valor = cfg.factorMeditacionClase(clase) * inteligencia * segundos;

    return std::max(0.0f, valor);
}
