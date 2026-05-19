#ifndef REGLAS_JUEGO_H
#define REGLAS_JUEGO_H

#include <cstdint>

#include "../config/config_juego.h"
#include "../modelo/raza.h"
#include "../modelo/clase_personaje.h"

class ReglasJuego {
public:
    static uint16_t calcularVidaMaxima(const ConfigJuego& cfg,
                                       Raza raza,
                                       ClasePersonaje clase,
                                       uint16_t nivel,
                                       uint16_t constitucion);

    static uint16_t calcularManaMaximo(const ConfigJuego& cfg,
                                       Raza raza,
                                       ClasePersonaje clase,
                                       uint16_t nivel,
                                       uint16_t inteligencia);

    static uint32_t calcularLimiteExperiencia(const ConfigJuego& cfg,
                                              uint16_t nivel);

    static uint32_t calcularOroSeguro(const ConfigJuego& cfg,
                                      uint16_t nivel);

    static uint32_t calcularOroMaximoTotal(const ConfigJuego& cfg,
                                           uint16_t nivel);

    static float calcularRecuperacionNatural(const ConfigJuego& cfg,
                                             Raza raza,
                                             float segundos);

    static float calcularRecuperacionMeditacion(const ConfigJuego& cfg,
                                                ClasePersonaje clase,
                                                uint16_t inteligencia,
                                                float segundos);
};

#endif
