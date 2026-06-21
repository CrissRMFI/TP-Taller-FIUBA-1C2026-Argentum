#include "registro_servidor.h"

#include <iostream>

void RegistroServidor::info(const std::string& mensaje) {
    std::cout << mensaje << std::endl;
}

void RegistroServidor::error(const std::string& mensaje) {
    std::cerr << mensaje << std::endl;
}

void RegistroServidor::errorCargandoJugador(const std::string& nombre, const std::string& detalle) {
    std::cerr << "[persistencia] error cargando " << nombre << ": " << detalle << std::endl;
}

void RegistroServidor::errorGuardandoJugador(const std::string& nombre, const std::string& detalle) {
    std::cerr << "[persistencia] error guardando " << nombre << ": " << detalle << std::endl;
}

void RegistroServidor::jugadorAtacaJugador(uint16_t idAtacante, uint16_t idObjetivo) {
    std::cout << "El jugador con id " << idAtacante << " ataca al jugador con id " << idObjetivo
              << std::endl;
}

void RegistroServidor::ataqueDistintoMapa() {
    std::cout << "Atacante y objetivo no están en el mismo mapa" << std::endl;
}

void RegistroServidor::ataqueEnZonaSegura() {
    std::cout << "Atacante o objetivo están en zona segura" << std::endl;
}

void RegistroServidor::ataqueEntreNewbies() {
    std::cout << "Atacante o objetivo son nuevos" << std::endl;
}

void RegistroServidor::ataqueDiferenciaNivelExcesiva(int diferencia) {
    std::cout << "La diferencia de nivel entre atacante y objetivo es demasiado grande: "
              << diferencia << std::endl;
}

void RegistroServidor::ataqueHechizoNoOfensivo() {
    std::cout << "El atacante intenta usar un hechizo no ofensivo" << std::endl;
}

void RegistroServidor::ataqueFueraDeRango(int distancia, int alcanceMaximo) {
    std::cout << "El objetivo está fuera del alcance del ataque: distancia " << distancia
              << ", alcance máximo " << alcanceMaximo << std::endl;
}

void RegistroServidor::ataqueManaInsuficiente() {
    std::cout << "El atacante no tiene mana suficiente" << std::endl;
}

void RegistroServidor::jugadorImpactaJugador(uint16_t idAtacante, uint16_t idObjetivo,
                                             uint16_t danio) {
    std::cout << "El atacante con id " << idAtacante << " ataca al jugador con id " << idObjetivo
              << " y causa " << danio << " de daño" << std::endl;
}

void RegistroServidor::jugadorMurio(uint16_t idJugador) {
    std::cout << "El jugador con id " << idJugador << " ha muerto" << std::endl;
}

void RegistroServidor::criaturaAtaqueDistintoMapa() {
    std::cerr << "Error: atacante y criatura no están en el mismo mapa." << std::endl;
}

void RegistroServidor::criaturaAtaqueZonaSegura() {
    std::cerr << "Error: atacante o criatura están en zona segura." << std::endl;
}

void RegistroServidor::criaturaAtaqueHechizoNoOfensivo() {
    std::cerr << "Error: el atacante intenta usar un hechizo no ofensivo." << std::endl;
}

void RegistroServidor::criaturaAtaqueFueraDeRango() {
    std::cerr << "Error: el objetivo está fuera del alcance del ataque." << std::endl;
}

void RegistroServidor::criaturaAtaqueManaInsuficiente() {
    std::cerr << "Error: el atacante no tiene mana suficiente." << std::endl;
}

void RegistroServidor::jugadorAtacaCriatura(uint16_t idAtacante, uint16_t idCriatura) {
    std::cout << "El jugador con id " << idAtacante << " ataca a criatura con id " << idCriatura
              << std::endl;
}

void RegistroServidor::criaturaMurio(uint16_t idCriatura) {
    std::cout << "La criatura con id " << idCriatura << " ha muerto" << std::endl;
}
