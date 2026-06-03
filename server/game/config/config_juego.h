#ifndef CONFIG_JUEGO_H
#define CONFIG_JUEGO_H

#include "../modelo/clase_personaje.h"
#include "../modelo/raza.h"
#include "../../../common/game/modelo/posicion.h"
#include <cstdint>
#include <string>
#include <vector>

// Una entrada del stock de un comerciante: que item vende, a que precio lo vende (compra, lo que paga el jugador) y a que precio lo recompra (venta).
struct EntradaStockComerciante {
    uint16_t id;
    uint8_t  precioCompra;
    uint8_t  precioVenta;
};

// Una entrada del stock de un sacerdote: que item vende y a que precio (el
// sacerdote no recompra items).
struct EntradaStockSacerdote {
    uint16_t id;
    uint8_t  precio;
};

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
    float oroMaxBase;     // OroMax = oroMaxBase * Nivel^oroMaxExp
    float oroMaxExp;
    float oroExcesoPct;   // fracción adicional permitida antes de ser "exceso"
    float oroDropNpcMax;  // rand(0, oroDropNpcMax) * VidaMaxNPC

    // ---- Combate ----
    float probabilidadCritico; // probabilidad entre 0.0 y 1.0
    float esquivarUmbral;     // esquiva si rand(0,1)^Agilidad < esquivarUmbral
    float cooldownAtaqueSeg;  // segundos minimos entre dos ataques del mismo jugador

    // ---- Fair play ----
    int nivelNewbie;      // nivel <= nivelNewbie: no puede atacar ni ser atacado
    int maxDiffNivel;     // |nivelA - nivelB| > maxDiffNivel: no pueden atacarse

    // ---- Inventario ----
    uint8_t inventarioMaxItems;
    uint16_t tiempoItemSueloSeg;

    // ---- Clanes ----
    int clanMaxMiembros;
    int clanNivelMinimo;
    uint16_t clanRadioBonus;
    float bonusClanPorAliado;

    // ---- Criatiruas ----
    uint16_t movimientoCriaturasTicks;
    uint16_t spawnCriaturasTicks;
    uint16_t poblacionMaxCriaturas;
    uint16_t criaturaVidaMaximaBase;
    uint8_t criaturaNivelBase;
    uint8_t criaturaFuerzaBase;
    uint8_t criaturaAgilidadBase;
    uint8_t criaturaRangoAggroBase;
    uint8_t criaturaDanioMinBase;
    uint8_t criaturaDanioMaxBase;

    // ---- Muerte / resurrección ----
    float expPerdidaMuertePct;      // fracción de experiencia perdida al morir
    float factorTiempoResurreccion; // segundos_inmovilizado = distancia * factor

    // ---- Servidor ----
    int tickMs;         // duración de cada tick en ms (también define TICK_SEGUNDOS)

    // ---- Persistencia de jugadores ----
    std::string rutaJugadores;        // archivo binario de datos (RegistroJugador)
    std::string rutaIndiceJugadores;  // archivo binario del indice nombre -> offset
    int         guardadoSeg;          // cada cuantos segundos se persiste (0 = off)

    // ---- Mapa ----
    uint16_t mapaAncho;
    uint16_t mapaAlto;
    std::string mapaArchivo;  // ruta del .bin (formato AOM1) que carga el servidor

    // Posicion ancla donde aparecen los jugadores al conectarse. La celda real la resuelve Juego al conectar (puede ser una vecina si esta ocupada).
    Posicion spawnInicial;

    // ---- NPCs ----
    uint16_t rangoInteraccionNpc;
    // Stock por tipo de NPC (todos los comerciantes/sacerdotes venden lo mismo).
    std::vector<EntradaStockComerciante> stockComerciante;
    std::vector<EntradaStockSacerdote>   stockSacerdote;

    // ---- Rangos de ataque (regla 5.3) ----
    // Alcance máximo en celdas para armas a distancia y hechizos de báculo.
    // El melee siempre es adyacencia (distancia Manhattan == 1).
    uint16_t rangoVisionAtaque;

    // ---- Cheats (para testing/corrección) ----
    bool vidaInfinita;
    bool manaInfinito;
    bool invulnerable;
    bool suicidio;
    bool expX10;

    float            factorVidaClase(ClasePersonaje clase) const;
    float            factorManaClase(ClasePersonaje clase) const;
    float            factorMeditacionClase(ClasePersonaje clase) const;
    const StatsRaza& statsRaza(Raza raza) const;
};

#endif
