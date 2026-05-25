#ifndef REGLAS_JUEGO_H
#define REGLAS_JUEGO_H

#include <cstddef>
#include <cstdint>

#include "../config/config_juego.h"
#include "../modelo/raza.h"
#include "../modelo/clase_personaje.h"

class ReglasJuego {
public:
    static uint16_t calcularVidaMaxima(const ConfigJuego& cfg, Raza raza, ClasePersonaje clase, uint16_t nivel, uint16_t constitucion);

    static uint16_t calcularManaMaximo(const ConfigJuego& cfg, Raza raza, ClasePersonaje clase, uint16_t nivel, uint16_t inteligencia);

    static uint32_t calcularLimiteExperiencia(const ConfigJuego& cfg, uint16_t nivel);

    static uint32_t calcularOroSeguro(const ConfigJuego& cfg, uint16_t nivel);

    static uint32_t calcularOroMaximoTotal(const ConfigJuego& cfg, uint16_t nivel);

    static float calcularRecuperacionNatural(const ConfigJuego& cfg, Raza raza, float segundos);

    static float calcularRecuperacionMeditacion(const ConfigJuego& cfg, ClasePersonaje clase, uint16_t inteligencia, float segundos);

    static bool esGolpeCritico(const ConfigJuego& cfg, float valorAleatorio);

    static uint32_t calcularPerdidaExperienciaMuerte(const ConfigJuego& cfg, uint32_t experienciaActual);

    static uint32_t calcularExperienciaImpacto(const ConfigJuego& cfg,
                                               uint16_t danioAplicado,
                                               uint8_t nivelAtacante,
                                               uint8_t nivelObjetivo);

    static uint32_t calcularExperienciaKill(const ConfigJuego& cfg,
                                            uint16_t vidaMaxObjetivo,
                                            uint8_t nivelAtacante,
                                            uint8_t nivelObjetivo,
                                            float valorAleatorio);

    static uint32_t calcularDropOroNpc(const ConfigJuego& cfg,
                                       uint16_t vidaMaxNpc,
                                       float valorAleatorio);

    static float calcularMultiplicadorClan(const ConfigJuego& cfg,
                                           size_t aliadosCercanos);

    static uint16_t aplicarMultiplicadorCombate(uint16_t valor,
                                                float multiplicador);

    // Defensor esquiva si pow(rand(0,1), Agilidad) < umbral. valorAleatorio debe venir uniforme en [0, 1).
    static bool esquivaAtaque(const ConfigJuego& cfg,
                              uint8_t agilidad,
                              float valorAleatorio);
};

#endif
