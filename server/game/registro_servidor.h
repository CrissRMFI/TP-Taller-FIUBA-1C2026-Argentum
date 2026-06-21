#ifndef REGISTRO_SERVIDOR_H
#define REGISTRO_SERVIDOR_H

#include <cstdint>
#include <string>

// Concentra la salida por consola del servidor (cout/cerr) para que la logica
// de juego (Juego) no haga I/O directo. Cada metodo escribe un evento/traza.
class RegistroServidor {
public:
    // --- Genericos (infra: red, ciclo de vida de clientes, fatales) ---
    static void info(const std::string& mensaje);   // a stdout
    static void error(const std::string& mensaje);  // a stderr

    // --- Persistencia ---
    static void errorCargandoJugador(const std::string& nombre, const std::string& detalle);
    static void errorGuardandoJugador(const std::string& nombre, const std::string& detalle);

    // --- Combate jugador vs jugador (trazas) ---
    static void jugadorAtacaJugador(uint16_t idAtacante, uint16_t idObjetivo);
    static void ataqueDistintoMapa();
    static void ataqueEnZonaSegura();
    static void ataqueEntreNewbies();
    static void ataqueDiferenciaNivelExcesiva(int diferencia);
    static void ataqueHechizoNoOfensivo();
    static void ataqueFueraDeRango(int distancia, int alcanceMaximo);
    static void ataqueManaInsuficiente();
    static void jugadorImpactaJugador(uint16_t idAtacante, uint16_t idObjetivo, uint16_t danio);
    static void jugadorMurio(uint16_t idJugador);

    // --- Combate jugador vs criatura (trazas) ---
    static void criaturaAtaqueDistintoMapa();
    static void criaturaAtaqueZonaSegura();
    static void criaturaAtaqueHechizoNoOfensivo();
    static void criaturaAtaqueFueraDeRango();
    static void criaturaAtaqueManaInsuficiente();
    static void jugadorAtacaCriatura(uint16_t idAtacante, uint16_t idCriatura);
    static void criaturaMurio(uint16_t idCriatura);

    RegistroServidor() = delete;
};

#endif
