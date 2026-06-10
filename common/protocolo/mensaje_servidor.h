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
    uint8_t  raza = 0;
    uint8_t  clase = 0;
    uint32_t expSiguienteNivel = 0;
    uint16_t tiempoResurreccionMs = 0;
};

struct MensajePosicionEntidad {
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
};

struct MensajeEntidadDesaparecio {
    uint16_t id;
};

struct MensajeDanoRecibido {
  uint16_t cantidad;
  uint16_t idAtacante;
  uint8_t  esCritico = 0;
};

struct MensajeDanoProducido {
  uint16_t cantidad;
  uint16_t idObjetivo;
  uint8_t  tipoGolpe;
  uint8_t  esCritico = 0;  // 1 = golpe critico
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
  std::vector<uint16_t> slots_;
};

enum class ErrorUsuario : uint8_t {
    NombreUsuarioNoEncontrado = 0,
    NickYaExistente = 1,
    UsuarioYaConectado = 2,
    Ninguno = 3,
};

struct MensajeEstadoUsuario {
  uint16_t id;
  std::string nick;
  ErrorUsuario error;
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
    Abandono         = 10,
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

struct MensajeContenidoBanco {
  std::vector<uint16_t> items;
  uint32_t oroBanco;
};

struct MensajeListaHechizos {
  std::vector<uint16_t> ids;  // hechizos conocidos del jugador
};

struct MensajeFxHechizo {
  uint16_t idHechizo;
  uint16_t idObjetivo;
};

struct MensajeProyectil {
  uint16_t idOrigen;
  uint16_t idDestino;
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
        MensajeListaItems,
        MensajeContenidoBanco,
        MensajeListaHechizos,
        MensajeFxHechizo,
        MensajeProyectil,
        MensajeEstadoUsuario>;


struct MensajeServidor {
    Opcode         opcode;
    PayloadMensajeServidor payload;
};


#endif
