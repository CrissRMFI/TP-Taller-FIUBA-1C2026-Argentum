#ifndef TALLER_TP_CLIENT_GAME_WORLD_H
#define TALLER_TP_CLIENT_GAME_WORLD_H
#include <cstdint>
#include <deque>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "client/entidad_renderizable.h"
#include "estado_chat_render.h"
#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"

struct EntityAnimationState {
    bool is_moving = false;
    uint32_t last_motion_tick = 0;
    int animation_row = 0;
    float previous_x = 0.0f;
    float previous_y = 0.0f;
    float current_x = 0.0f;
    float current_y = 0.0f;
    uint32_t move_start_tick = 0;
};

// para que la animacion no quede atada a tiles interpolo posiciones (x,y) del personaje
// buscando un punto medio entre la pos actual y la anterior

struct InterpolatedPosition {
    float x = 0.0f;
    float y = 0.0f;
};

// Lo que el jugador tiene equipado 
struct EquipamientoJugador {
    uint16_t arma = 0;
    uint16_t baculo = 0;
    uint16_t defensa = 0;
    uint16_t casco = 0;
    uint16_t escudo = 0;
};

// Stats del jugador para mostrar en el panel.
struct EstadoJugador {
    uint16_t vida = 0;
    uint16_t vidaMax = 0;
    uint16_t mana = 0;
    uint16_t manaMax = 0;
    uint32_t oro = 0;
    uint32_t experiencia = 0;
    uint8_t  nivel = 0;
    uint8_t  raza = 0;
    uint8_t  clase = 0;
    uint32_t expSiguienteNivel = 0;
};

class GestorAudio;

class ObjectGameWorld {
private:
    std::unordered_map<uint16_t, EntidadRenderizable> entidades;
    std::unordered_map<uint16_t, EntityAnimationState> animation_states;

    uint16_t idCliente;
    int posX;
    int posY;

    uint8_t nivelAnterior;
    uint8_t estadoAnterior;
    bool vidaBajaAvisada;
    uint16_t vidaAnterior;
    uint16_t manaAnterior;
    uint32_t experienciaAnterior = 0;
    uint32_t oroAnterior = 0;

    // Ultimas lineas de chat/feedback recibidas del server (con su tipo/color)
    std::deque<LineaChat> historialChatReciente;
    size_t maxLineasChat = 6;
    std::string nick_;  // nombre del propio jugador (se conoce al loguear)

    // Estado de objetos del jugador (llega por protocolo; lo usa el panel derecho).
    std::vector<uint16_t> inventario_;   // ids por slot (0 = vacio)
    EquipamientoJugador   equipamiento_;
    EstadoJugador         estadoJugador_;
    std::vector<uint16_t> stockNpc_;      // ultimo /listar de un comerciante (ids en venta)
    std::vector<uint16_t> bancoItems_;    // items guardados en el banco (ultimo CONTENIDO_BANCO)
    uint32_t              bancoOro_ = 0;   // oro guardado en el banco
    bool                  bancoRecibido_ = false;  // llego un CONTENIDO_BANCO (banco abierto)
    std::vector<uint16_t> hechizosConocidos_;       // ids de hechizos del jugador (LISTA_HECHIZOS)
    // Drops visibles en el piso (celda x,y). Oro -> monedas; item -> bolsa/cofre.
    std::set<std::pair<uint16_t, uint16_t>> oroEnSuelo_;
    std::set<std::pair<uint16_t, uint16_t>> itemEnSuelo_;
    // FX de hechizos recibidos del server (idHechizo, idObjetivo); el loop los pasa al renderer.
    std::vector<std::pair<uint16_t, uint16_t>> fxPendientes_;
    // Proyectiles recibidos (idOrigen, idDestino); el loop los pasa al renderer.
    std::vector<std::pair<uint16_t, uint16_t>> proyectilesPendientes_;

    int distanciaAlJugador(int x, int y) const;
    void agregarLineaChat(const std::string& linea, TipoMensajeChat tipo = TipoMensajeChat::Normal);

public:
    explicit ObjectGameWorld(uint16_t client_id);
    void upload_server_msg(Queue<MensajeServidor>& server_msgs, uint32_t current_tick,
                           GestorAudio& gestorAudio);
    void notify_move_requested(uint32_t current_tick);

    const std::unordered_map<uint16_t, EntidadRenderizable>& entities() const;
    uint16_t client_id() const;
    int player_x() const;
    int player_y() const;
    bool player_is_moving() const;
    bool entity_is_moving(uint16_t entity_id) const;
    int entity_animation_row(uint16_t entity_id) const;
    InterpolatedPosition entity_interpolated_position(uint16_t entity_id,
                                                      uint32_t current_tick) const;
    const std::deque<LineaChat>& historialChat() const;
    void setMaxLineasChat(size_t maximo);
    void setNick(const std::string& nick) { nick_ = nick; }
    const std::string& nick() const { return nick_; }
    const std::set<std::pair<uint16_t, uint16_t>>& oroEnSuelo() const { return oroEnSuelo_; }
    const std::set<std::pair<uint16_t, uint16_t>>& itemEnSuelo() const { return itemEnSuelo_; }
    // Para que el loop agregue mensajes locales con color (ej. lanzar hechizo, "no hay nada").
    void mensajeLocal(const std::string& linea, TipoMensajeChat tipo);

    const std::vector<uint16_t>& inventario() const;
    const EquipamientoJugador& equipamiento() const;
    const EstadoJugador& estadoJugador() const;
    const std::vector<uint16_t>& stockNpc() const;
    const std::vector<uint16_t>& bancoItems() const;
    uint32_t bancoOro() const;
    bool bancoRecibido() const;
    void cerrarBanco();
    const std::vector<uint16_t>& hechizosConocidos() const;
    // Devuelve y limpia los FX de hechizo recibidos desde el ultimo frame.
    std::vector<std::pair<uint16_t, uint16_t>> drenarFx();
    std::vector<std::pair<uint16_t, uint16_t>> drenarProyectiles();
};


#endif

