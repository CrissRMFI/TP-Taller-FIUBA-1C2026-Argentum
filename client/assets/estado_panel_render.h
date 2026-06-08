#ifndef TALLER_TP_ESTADO_PANEL_RENDER_H
#define TALLER_TP_ESTADO_PANEL_RENDER_H

#include <cstdint>
#include <string>
#include <vector>

#include <SDL_pixels.h>

#include "client_game_world.h"

struct ConfigPanelRender {
    int ancho = 268;
    std::string iconDir = "imgs/items";
    std::string fondoCuero = "imgs/ui/leather_brown.png";
    std::string marcoInventario = "imgs/ui/es_centroinventario.bmp";
    std::string marcoHechizos = "imgs/hechizos/es_centrohechizo.bmp";
    std::string botonVender = "imgs/ui/es_boton-vender-default.bmp";
    std::string botonEquipar = "imgs/ui/es_boton-construir-default.bmp";
    std::string botonUsar = "imgs/ui/es_boton-usar.png";
    std::string botonCurar = "imgs/ui/es_boton-curar.bmp";
    std::string barraVida = "imgs/ui/en_barradevida.bmp";
    std::string barraMana = "imgs/ui/en_barrademana.bmp";
    std::string barraExperiencia = "imgs/estados/es_barraexperiencia.bmp";
    std::string spriteMeditacion = "imgs/estados/3446.png"; // Aura meditar
    std::string bancoImg = "imgs/ui/es_banco.bmp";
    std::string botonDepositar = "imgs/ui/es_boton-depositar.bmp";
    std::string botonRetirar = "imgs/ui/es_boton-retirar.bmp";
    std::string botonDepositarOro = "imgs/ui/es_boton-depositar-oro.bmp";
    std::string botonRetirarOro = "imgs/ui/es_boton-retirar-oro.bmp";
    // Grilla del banco (relativa a la esquina sup-izq de la ventana es_banco.bmp).
    // Calibrable por TOML [banco] porque depende de los recuadros del asset.
    int bancoBovedaX = 45;
    int bancoBovedaY = 127;
    int bancoInvX = 305;
    int bancoInvY = 127;
    int bancoSlot = 32;
    int bancoGap = 4;
    int bancoCols = 6;
    SDL_Color   colorTexto = {235, 225, 200, 255};
    SDL_Color   colorTitulo = {255, 220, 120, 255};
};


// Estado por-frame de la ventana modal del banco.
struct EstadoBancoRender {
    bool abierto = false;
    std::vector<uint16_t> boveda;
    std::vector<uint16_t> inventario;
    uint32_t oroBanco = 0;
    uint32_t oroJugador = 0;
    int selBoveda = -1;
    int selInventario = -1;
    std::string monto;
    bool montoActivo = false; // la caja de monto tiene foco
};


struct EstadoPanelRender {
    std::string nick;   // nombre del jugador (titulo del panel)
    std::string raza;   // nombre de la raza (resuelto desde el enum)
    std::string clase;  // nombre de la clase
    std::vector<uint16_t> inventario;
    EquipamientoJugador equip;
    EstadoJugador stats;
    std::vector<uint16_t> stock;
    int seleccionInventario = -1;
    int scrollStock = 0;
    std::vector<uint16_t> hechizosConocidos;  // ids que el jugador conoce (pestaña HECHIZOS = lanzar)
    bool mostrarHechizos = false;             // pestaña activa: false=inventario, true=hechizos
    bool sacerdoteSeleccionado = false;       // objetivo es un sacerdote -> ofrece hechizos en venta
};

#endif
