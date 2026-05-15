#ifndef CONFIG_JUEGO_H
#define CONFIG_JUEGO_H

#include "../modelo/clase_personaje.h"
#include "../modelo/raza.h"

// Stats base de una raza: valores absolutos y factores multiplicadores
// que se usan en las ecuaciones de vida, mana y recuperación.
struct StatsRaza {
    float fVida;          // FRazaVida
    float fMana;          // FRazaMana
    float fRecuperacion;  // FRazaRecuperacion
    int   constitucion;
    int   fuerza;
    int   agilidad;
    int   inteligencia;
};

struct ConfigJuego {
    // VidaMax = Constitución * fClaseVida * fRazaVida * Nivel
    float fVidaGuerrero;
    float fVidaPaladin;
    float fVidaClerigo;
    float fVidaMago;

    // ManaMax = Inteligencia * fClaseMana * fRazaMana * Nivel
    float fManaGuerrero;   // siempre 0: el guerrero no tiene maná
    float fManaPaladin;
    float fManaClerigo;
    float fManaMago;

    // Mana/seg = fClaseMeditacion * Inteligencia
    float fMeditacionPaladin;
    float fMeditacionClerigo;
    float fMeditacionMago;

    // ---- Stats base por raza ----
    StatsRaza humano;
    StatsRaza elfo;
    StatsRaza enano;
    StatsRaza gnomo;

    // ---- Experiencia ----
    int   expLimiteBase;  // Limite = expLimiteBase * Nivel^expLimiteExp
    float expLimiteExp;
    int   expBonusNivel;  // max(NivelOtro - Nivel + expBonusNivel, 0)
    float expKillMax;     // rand(0, expKillMax) * VidaMaxOtro * factorNivel

    // ---- Oro ----
    float oroMaxExp;      // OroMax = 100 * Nivel^oroMaxExp
    float oroExcesoPct;   // fracción adicional permitida antes de ser "exceso"
    float oroDropNpcMax;  // rand(0, oroDropNpcMax) * VidaMaxNPC

    // ---- Combate ----
    float esquivarUmbral; // esquiva si rand(0,1)^Agilidad < esquivarUmbral

    // ---- Fair play ----
    int nivelNewbie;      // nivel <= nivelNewbie: no puede atacar ni ser atacado
    int maxDiffNivel;     // |nivelA - nivelB| > maxDiffNivel: no pueden atacarse

    // ---- Clanes ----
    int clanMaxMiembros;
    int clanNivelMinimo;

    // ---- Muerte / resurrección ----
    float factorTiempoResurreccion; // segundos_inmovilizado = distancia * factor

    // ---- Cheats (para testing/corrección) ----
    bool vidaInfinita;
    bool manaInfinito;
    bool invulnerable;
    bool expX10;

    float            factorVidaClase(ClasePersonaje clase) const;
    float            factorManaClase(ClasePersonaje clase) const;
    float            factorMeditacionClase(ClasePersonaje clase) const;
    const StatsRaza& statsRaza(Raza raza) const;
};

#endif
