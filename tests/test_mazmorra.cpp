#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

#include "../common/game/mapa/mundo.h"
#include "../common/persistencia/lector_mapa.h"

namespace {

// Escribe un escenario TOML temporal (exterior + una mazmorra) y devuelve su ruta.
std::string escribirEscenarioTemporal() {
    const std::string ruta = "test_mazmorra_tmp.toml";
    std::ofstream archivo(ruta, std::ios::trunc);
    archivo <<
            "mapa_id = 0\n"
            "ancho = 100\n"
            "alto = 100\n"
            "\n"
            "[[mazmorra]]\n"
            "mapa_id = 1\n"
            "ancho = 40\n"
            "alto = 40\n"
            "entrada = { x = 70, y = 50, destino_x = 2, destino_y = 2 }\n"
            "salida  = { x = 1, y = 1, destino_x = 70, destino_y = 51 }\n"
            "pisos = [ { x_min = 0, y_min = 0, x_max = 39, y_max = 39, clave = \"caverna\" } ]\n";
    return ruta;
}

} 

// leerMundo carga el exterior y la mazmorra como dos mapas distintos.
TEST(Mazmorra, LeerMundoCargaExteriorYMazmorra) {
    const std::string ruta = escribirEscenarioTemporal();
    LectorMapa lector;
    WorldCargado mundo = lector.leerMundo(ruta);
    std::remove(ruta.c_str());

    EXPECT_EQ(mundo.mapaPrincipalId, 0);
    ASSERT_EQ(mundo.mapas.size(), 2u);
    ASSERT_EQ(mundo.mapas.count(0), 1u);
    ASSERT_EQ(mundo.mapas.count(1), 1u);
    EXPECT_EQ(mundo.mapas.at(0).getAncho(), 100);
    EXPECT_EQ(mundo.mapas.at(1).getAncho(), 40);
    EXPECT_EQ(mundo.mapas.at(1).getAlto(), 40);
}

// La entrada y la salida producen dos portales con sus mapaId bien armados.
TEST(Mazmorra, LeerMundoArmaPortalesEntradaYSalida) {
    const std::string ruta = escribirEscenarioTemporal();
    LectorMapa lector;
    WorldCargado cargado = lector.leerMundo(ruta);
    std::remove(ruta.c_str());

    ASSERT_EQ(cargado.portales.size(), 2u);

    Mundo mundo(std::move(cargado.mapas), std::move(cargado.portales),
                cargado.mapaPrincipalId);

    EXPECT_TRUE(mundo.existeMapa(1));
    EXPECT_FALSE(mundo.existeMapa(2));

    // Entrada: celda (70,50) del exterior -> (2,2) de la mazmorra (mapaId 1).
    std::optional<Posicion> aMazmorra = mundo.destinoPortalEn(Posicion{70, 50, 0});
    ASSERT_TRUE(aMazmorra.has_value());
    EXPECT_EQ(aMazmorra->x, 2);
    EXPECT_EQ(aMazmorra->y, 2);
    EXPECT_EQ(aMazmorra->mapaId, 1);

    // Salida: celda (1,1) de la mazmorra -> (70,51) del exterior (mapaId 0).
    std::optional<Posicion> alExterior = mundo.destinoPortalEn(Posicion{1, 1, 1});
    ASSERT_TRUE(alExterior.has_value());
    EXPECT_EQ(alExterior->x, 70);
    EXPECT_EQ(alExterior->y, 51);
    EXPECT_EQ(alExterior->mapaId, 0);

    // Una celda sin portal no transporta.
    EXPECT_FALSE(mundo.destinoPortalEn(Posicion{0, 0, 0}).has_value());
}

// leer() (mapa principal, lo usan cliente y editor) ignora las mazmorras.
TEST(Mazmorra, LeerPrincipalIgnoraMazmorras) {
    const std::string ruta = escribirEscenarioTemporal();
    LectorMapa lector;
    MapaCargado principal = lector.leer(ruta);
    std::remove(ruta.c_str());

    EXPECT_EQ(principal.mapaId, 0);
    EXPECT_EQ(principal.mapa.getAncho(), 100);
    EXPECT_EQ(principal.mapa.getAlto(), 100);
}
