#include "config_juego.h"

float ConfigJuego::factorVidaClase(ClasePersonaje clase) const {
    switch (clase) {
        case ClasePersonaje::GUERRERO: return fVidaGuerrero;
        case ClasePersonaje::PALADIN:  return fVidaPaladin;
        case ClasePersonaje::CLERIGO:  return fVidaClerigo;
        case ClasePersonaje::MAGO:     return fVidaMago;
    }
    return fVidaMago;
}

float ConfigJuego::factorManaClase(ClasePersonaje clase) const {
    switch (clase) {
        case ClasePersonaje::GUERRERO: return fManaGuerrero;
        case ClasePersonaje::PALADIN:  return fManaPaladin;
        case ClasePersonaje::CLERIGO:  return fManaClerigo;
        case ClasePersonaje::MAGO:     return fManaMago;
    }
    return fManaMago;
}

// El guerrero no puede meditar: devuelve 0 para que cualquier cálculo que lo
// llame por error produzca recuperación nula en lugar de comportamiento indefinido.
float ConfigJuego::factorMeditacionClase(ClasePersonaje clase) const {
    switch (clase) {
        case ClasePersonaje::GUERRERO: return 0.0f;
        case ClasePersonaje::PALADIN:  return fMeditacionPaladin;
        case ClasePersonaje::CLERIGO:  return fMeditacionClerigo;
        case ClasePersonaje::MAGO:     return fMeditacionMago;
    }
    return 0.0f;
}

const StatsRaza& ConfigJuego::statsRaza(Raza raza) const {
    switch (raza) {
        case Raza::HUMANO: return humano;
        case Raza::ELFO:   return elfo;
        case Raza::ENANO:  return enano;
        case Raza::GNOMO:  return gnomo;
    }
    return humano;
}
