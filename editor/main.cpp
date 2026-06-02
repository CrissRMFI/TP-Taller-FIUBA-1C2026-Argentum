#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

#include "common/persistencia/escritor_mapa.h"

#include "mapaCreator.h"

// Generador de mapas en binario (formato AOM1). Por ahora produce el mapa generico hardcodeado por MapaCreator; mas adelante lo reemplazado por el editor grafico Qt. 
int main(int argc, char* argv[]) {
    const std::string salida = (argc >= 2) ? argv[1] : "config/mapa.bin";
    const uint16_t ancho = (argc >= 3) ? static_cast<uint16_t>(std::stoi(argv[2])) : 100;
    const uint16_t alto = (argc >= 4) ? static_cast<uint16_t>(std::stoi(argv[3])) : 100;
    const uint16_t mapaId = 0;

    try {
        MapaCreator creator;
        Mapa mapa = creator.crearMapaGenerico(ancho, alto);
        EscritorMapa::escribir(mapa, mapaId, salida);
        std::cout << "Mapa generico escrito en '" << salida << "' ("
                  << ancho << "x" << alto << ", mapaId=" << mapaId << ")\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error al generar el mapa: " << e.what() << "\n";
        return 1;
    }
}
