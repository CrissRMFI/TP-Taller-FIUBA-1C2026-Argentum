#ifndef MENSAJE_SERVIDOR_H
#define MENSAJE_SERVIDOR_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include "../mensajes/codigo_error_accion.h"

#include "./opcode.h"

struct MensajeEstadoPersonaje {
    uint16_t vidaActual;
    uint16_t vidaMax;
    uint16_t manaActual;
    uint16_t manaMax;
    uint32_t oro;
    uint8_t  nivel;
    uint32_t experiencia;
    uint8_t  estado;
};

struct MensajePosicionEntidad {
    uint16_t id;
    uint16_t x;
    uint16_t y;
    uint8_t tipo;
    uint8_t estado;
};

struct MensajeEntidadDesaparecio {
    uint16_t id;
};

struct MensajeDanoRecibido {
  uint16_t cantidad;
  uint16_t idAtacante;
};

struct MensajeDanoProducido {
  uint16_t cantidad;
  uint16_t idObjetivo;
};

struct MensajeEsquive {
  uint16_t idEntidad;
  uint8_t esquivador;
};

struct MensajeMuerteEntidad {
  uint16_t id;
};

struct MensajeItemEnSuelo {
  uint16_t idItem;
  uint16_t x;
  uint16_t y;
};

struct MensajeItemDesaparecioSuelo {
  uint16_t x;
  uint16_t y;
};

struct MensajeOroEnSuelo {
  uint32_t cantidad;
  uint16_t x;
  uint16_t y;
};

struct MensajeOroDesaparecioSuelo {
  uint16_t x;
  uint16_t y;
};

struct MensajeActualizarInventario {
  std::vector<uint16_t> slots;
};

struct MensajeActualizarEquipamiento {
  uint16_t arma;
  uint16_t baculo;
  uint16_t defensa;
  uint16_t casco;
  uint16_t escudo;
};

struct MensajeChat {
  std::string nickOrigen;
  std::string mensaje;
};

enum class TipoMensajeClan : uint8_t {
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
};

struct MensajeClan {
    TipoMensajeClan tipo;
    std::string texto;
};

struct MensajeResucitado {
  uint16_t x;
  uint16_t y;
};

struct MensajeListaItems {
  std::vector<uint16_t> ids;
};

struct MensajeErrorAccion {
    CodigoErrorAccion codigo;
};

using PayloadMensajeServidor = std::variant<
        MensajeEstadoPersonaje,
        MensajePosicionEntidad,
        MensajeEntidadDesaparecio,
        MensajeDanoRecibido,
        MensajeDanoProducido,
        MensajeEsquive,
        MensajeMuerteEntidad,
        MensajeItemEnSuelo,
        MensajeItemDesaparecioSuelo,
        MensajeOroEnSuelo,
        MensajeOroDesaparecioSuelo,
        MensajeActualizarInventario,
        MensajeActualizarEquipamiento,
        MensajeChat,
        MensajeClan,
        MensajeResucitado,
        MensajeErrorAccion,
        MensajeListaItems>;


struct MensajeServidor {
    Opcode         opcode;
    PayloadMensajeServidor payload;
};


#endif
