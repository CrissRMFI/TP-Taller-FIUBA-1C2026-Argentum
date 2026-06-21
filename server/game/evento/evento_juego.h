#ifndef EVENTO_JUEGO_H
#define EVENTO_JUEGO_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "../../../common/mensajes/codigo_error_accion.h"
#include "../../../common/protocolo/categoria_chat.h"
#include "../jugador.h"

struct EventoEstadoPersonaje {
    uint16_t vidaActual;
    uint16_t vidaMax;
    uint16_t manaActual;
    uint16_t manaMax;
    uint32_t oro;
    uint8_t nivel;
    uint32_t experiencia;
    Estado estado;
    uint8_t raza = 0;
    uint8_t clase = 0;
    uint32_t expSiguienteNivel = 0;     // experiencia necesaria para subir de nivel
    uint16_t tiempoResurreccionMs = 0;  // ms inmovil restantes al resucitar (0 si no resucita)
};

struct EventoPosicionEntidad {
    uint16_t id;
    uint16_t x;
    uint16_t y;
    uint8_t tipo;
    uint8_t estado;
    uint16_t cabeza;
    uint16_t cuerpo;
    uint16_t arma = 0;
    uint16_t escudo = 0;
    uint16_t casco = 0;
    uint16_t mapaId = 0;
};

struct EventoEntidadDesaparecio {
    uint16_t id;
};

struct EventoDanioRecibido {
    uint16_t cantidad;
    uint16_t idAtacante;
    bool esCritico = false;
};

struct EventoDanioProducido {
    uint16_t cantidad;
    uint16_t idObjetivo;
    uint8_t tipoGolpe;  // TipoGolpe: derivado del arma del atacante
    bool esCritico = false;
};

struct EventoEsquive {
    uint16_t idEntidad;
    uint8_t esquivador;
};

struct EventoMuerteEntidad {
    uint16_t id;
};

struct EventoItemEnSuelo {
    uint16_t idItem;
    uint16_t x;
    uint16_t y;
};

struct EventoItemDesaparecioSuelo {
    uint16_t x;
    uint16_t y;
};

struct EventoOroEnSuelo {
    uint32_t cantidad;
    uint16_t x;
    uint16_t y;
};

struct EventoOroDesaparecioSuelo {
    uint16_t x;
    uint16_t y;
};

struct EventoActualizarInventario {
    std::vector<uint16_t> slots;
};

struct EventoActualizarEquipamiento {
    uint16_t arma;
    uint16_t baculo;
    uint16_t defensa;
    uint16_t casco;
    uint16_t escudo;
};

struct EventoChat {
    std::string nickOrigen;
    std::string mensaje;
    uint8_t tipo = 0;
};

struct EventoResucitado {
    uint16_t x;
    uint16_t y;
};

struct EventoListaItems {
    std::vector<uint16_t> ids;
};

struct EventoContenidoBanco {
    std::vector<uint16_t> items;
    uint32_t oroBanco;
};

struct EventoListaHechizos {
    std::vector<uint16_t> ids;
};

struct EventoFxHechizo {
    uint16_t idHechizo;
    uint16_t idObjetivo;
};

struct EventoProyectil {
    uint16_t idOrigen;
    uint16_t idDestino;
};

struct EventoCambioMapa {
    uint16_t mapaId;
};

struct EventoErrorAccion {
    CodigoErrorAccion codigo;
};

using EventoJuego =
        std::variant<EventoEstadoPersonaje, EventoPosicionEntidad, EventoEntidadDesaparecio,
                     EventoDanioRecibido, EventoDanioProducido, EventoEsquive, EventoMuerteEntidad,
                     EventoItemEnSuelo, EventoItemDesaparecioSuelo, EventoOroEnSuelo,
                     EventoOroDesaparecioSuelo, EventoActualizarInventario,
                     EventoActualizarEquipamiento, EventoChat, EventoResucitado, EventoListaItems,
                     EventoContenidoBanco, EventoListaHechizos, EventoFxHechizo, EventoProyectil,
                     EventoCambioMapa, EventoErrorAccion>;

#endif
