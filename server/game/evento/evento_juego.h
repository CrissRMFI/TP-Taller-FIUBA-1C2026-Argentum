#ifndef EVENTO_JUEGO_H
#define EVENTO_JUEGO_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "../../../common/mensajes/codigo_error_accion.h"
#include "../jugador.h"

struct EventoEstadoPersonaje {
    uint16_t vidaActual;
    uint16_t vidaMax;
    uint16_t manaActual;
    uint16_t manaMax;
    uint32_t oro;
    uint8_t  nivel;
    uint32_t experiencia;
    Estado   estado;
};

struct EventoPosicionEntidad {
    uint16_t id;
    uint16_t x;
    uint16_t y;
    uint8_t  tipo;
    uint8_t  estado;
    uint16_t cabeza;
    uint16_t cuerpo;
};

struct EventoEntidadDesaparecio {
    uint16_t id;
};

struct EventoDanioRecibido {
    uint16_t cantidad;
    uint16_t idAtacante;
};

struct EventoDanioProducido {
    uint16_t cantidad;
    uint16_t idObjetivo;
    uint8_t  tipoGolpe;  // TipoGolpe: derivado del arma del atacante
};

struct EventoEsquive {
    uint16_t idEntidad;
    uint8_t  esquivador;
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
};

enum class TipoEventoClan : uint8_t {
    MiembroActivo    = 0,
    MiembroPendiente = 1,
    Fundado          = 2,
    Aceptado         = 3,
    Rechazado        = 4,
    Baneado          = 5,
    Kickeado         = 6,
    Conectado        = 7,
    Desconectado     = 8,
    BajoAtaque       = 9,
    Abandono         = 10,
};

struct EventoClan {
    TipoEventoClan tipo;
    std::string    texto;
};

struct EventoResucitado {
    uint16_t x;
    uint16_t y;
};

struct EventoListaItems {
    std::vector<uint16_t> ids;
};

struct EventoErrorAccion {
    CodigoErrorAccion codigo;
};

using EventoJuego = std::variant<
        EventoEstadoPersonaje,
        EventoPosicionEntidad,
        EventoEntidadDesaparecio,
        EventoDanioRecibido,
        EventoDanioProducido,
        EventoEsquive,
        EventoMuerteEntidad,
        EventoItemEnSuelo,
        EventoItemDesaparecioSuelo,
        EventoOroEnSuelo,
        EventoOroDesaparecioSuelo,
        EventoActualizarInventario,
        EventoActualizarEquipamiento,
        EventoChat,
        EventoClan,
        EventoResucitado,
        EventoListaItems,
        EventoErrorAccion>;

#endif
