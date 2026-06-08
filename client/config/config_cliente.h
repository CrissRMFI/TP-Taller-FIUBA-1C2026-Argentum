#ifndef CLIENT_CONFIG_CONFIG_CLIENTE_H
#define CLIENT_CONFIG_CONFIG_CLIENTE_H

#include <cstdint>
#include <string>
#include <vector>

struct ConfigCliente {
    bool     vsync = true;
    int      fpsMax = 60;
    uint32_t intervaloMovimientoMs = 130;

    int  ancho = 640;
    int  alto = 480;
    bool fullscreen = false;

    std::string fuenteRuta = "fonts/font.ttf";
    std::string fondoChatRuta = "chat/fondoPrincipal.png";
    int fuenteTam = 14;
    int chatMaxLineas = 6;
    int chatPanelX = 0;
    int chatPanelY = 0;
    int chatPanelAlto = 120; 
    std::vector<int> chatColorTexto = {255, 255, 255};
    std::vector<int> chatColorInput = {255, 255, 0};
    // Colores por tipo de mensaje de chat (centralizados, configurables).
    std::vector<int> chatColorAtaque = {220, 60, 60};
    std::vector<int> chatColorHechizo = {240, 150, 40};
    std::vector<int> chatColorSistema = {40, 120, 160};
    std::vector<int> chatColorExperiencia = {80, 120, 255};
    std::vector<int> chatColorRecuperacion = {40, 150, 60};
    std::vector<int> chatColorClan = {150, 100, 50};
    std::vector<int> chatColorCriticoHecho = {255, 200, 60};
    std::vector<int> chatColorCriticoRecibido = {255, 40, 100};

    // Nombres de raza/clase indexados por el enum del server (para mostrar en el panel).
    std::vector<std::string> razasNombres = {"Humano", "Elfo", "Enano", "Gnomo"};
    std::vector<std::string> clasesNombres = {"Mago", "Clerigo", "Paladin", "Guerrero"};

    // Panel derecho (inventario/equipo/stats/comercio).
    int              panelAncho = 268;
    std::string      panelIconDir = "imgs/items";
    std::string      panelFondoCuero = "imgs/ui/leather_brown.png";
    std::vector<int> panelColorTexto = {235, 225, 200};
    std::vector<int> panelColorTitulo = {255, 220, 120};

    // Grilla del banco (relativa a la ventana es_banco.bmp). Calibrable sin recompilar.
    int bancoBovedaX = 45;
    int bancoBovedaY = 127;
    int bancoInvX = 305;
    int bancoInvY = 127;
    int bancoSlot = 32;
    int bancoGap = 4;
    int bancoCols = 6;

    // Rango (distancia Manhattan en celdas) para considerar valida la seleccion de un
    // objetivo: si esta mas lejos, no se resalta.
    int seleccionRango = 10;
    // Radio en pixeles del area de click para seleccionar una criatura/NPC.
    int seleccionRadioPx = 40;
};

#endif
