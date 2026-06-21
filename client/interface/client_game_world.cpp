#include "client_game_world.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <variant>

#include "../../common/mensajes/mensajes_error_accion.h"
#include "../../common/protocolo/estado_entidad.h"
#include "../../common/protocolo/tipo_entidad.h"
#include "../../common/protocolo/tipo_golpe.h"
#include "../audio/gestor_audio.h"

#define MOTION_GRACE_MS 260

#define UMBRAL_VIDA_BAJA 20

#define UMBRAL_CURACION 5


ObjectGameWorld::ObjectGameWorld(const uint16_t client_id):
    idCliente(client_id), posX(0), posY(0),
    nivelAnterior(0),
    estadoAnterior(static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo)),
    vidaBajaAvisada(false), vidaAnterior(0), manaAnterior(0) {}

int ObjectGameWorld::animation_row_for_delta(const int delta_x, const int delta_y, const int default_row) {
    // No cambios en la posicion: me quedo en la fila actual.
    if (delta_x == 0 && delta_y == 0) {
        return default_row;
    }
    // Movimiento lateral.
    if (std::abs(delta_x) >= std::abs(delta_y)) {
        return (delta_x > 0) ? 2 : 1;
    }
    // Movimiento vertical.
    return (delta_y > 0) ? 0 : 3;
}

int ObjectGameWorld::distanciaAlJugador(int x, int y) const {
    return std::max(std::abs(x - posX), std::abs(y - posY));
}

void ObjectGameWorld::actualizarPosVisualEntidad(EntityAnimationState& animation_state,
                                                 const int tile_x,
                                                 const int tile_y,
                                                 const uint32_t current_tick) {

    // Interpolacion visual, las posiciones logicas de los jugadores son interpoladas
    // localmente con el fin de suavizar el movimiento

    const double speed = 1000.0 / 130.0;  // tiles por segundo

    if (!animation_state.vis_init) {
        animation_state.vis_x = tile_x;
        animation_state.vis_y = tile_y;
        animation_state.vis_init = true;
        animation_state.vis_last_tick = current_tick;
        return;
    }

    double dt = (current_tick - animation_state.vis_last_tick) / 1000.0;
    animation_state.vis_last_tick = current_tick;
    dt = std::clamp(dt, 0.0, 0.1);

    const double rem_x = tile_x - animation_state.vis_x;
    const double rem_y = tile_y - animation_state.vis_y;
    const double restante = std::abs(rem_x) + std::abs(rem_y);

    constexpr double EPS = 1e-4;
    if (restante < EPS) {
        return;
    }
    if (restante > 2.5) {
        animation_state.vis_x = tile_x;
        animation_state.vis_y = tile_y;
        return;
    }

    double paso = speed * dt;
    if (restante > 1.5) {
        paso *= 2.0;
    }

    const bool x_en_curso =
            std::abs(animation_state.vis_x - std::round(animation_state.vis_x)) > EPS;
    const auto avanzar = [&](double& v, const double objetivo) {
        const double d = objetivo - v;
        v = (std::abs(d) <= paso) ? objetivo : v + (d > 0 ? paso : -paso);
    };
    if (std::abs(rem_x) > EPS && (x_en_curso || std::abs(rem_y) <= EPS)) {
        avanzar(animation_state.vis_x, tile_x);
    } else {
        avanzar(animation_state.vis_y, tile_y);
    }

    animation_state.vis_x = tile_x;
    animation_state.vis_y = tile_y;
    animation_state.vis_init = true;
    animation_state.vis_last_tick = current_tick;
}

void ObjectGameWorld::upload_server_msg(Queue<MensajeServidor>& server_msgs,
                                            const uint32_t current_tick,
                                            GestorAudio& gestorAudio) {
    const int previous_pos_x = posX;
    const int previous_pos_y = posY;

    MensajeServidor mensaje;
    while (server_msgs.try_pop(mensaje)) {
        if (auto* entity_position = std::get_if<MensajePosicionEntidad>(&mensaje.payload)) {
            const auto previous_entity = entidades.find(entity_position->id);
            const bool esNueva = (previous_entity == entidades.end());
            const int previous_x =
                    (previous_entity != entidades.end()) ? previous_entity->second.x : entity_position->x;
            const int previous_y =
                    (previous_entity != entidades.end()) ? previous_entity->second.y : entity_position->y;
            const bool position_changed =
                    (entity_position->x != previous_x || entity_position->y != previous_y);

            entidades[entity_position->id] = EntidadRenderizable{entity_position->x,
                                                                 entity_position->y,
                                                                 entity_position->tipo,
                                                                 entity_position->estado,
                                                                 entity_position->cabeza,
                                                                 entity_position->cuerpo,
                                                                 entity_position->arma,
                                                                 entity_position->escudo,
                                                                 entity_position->casco,
                                                                 entity_position->mapaId};

            EntityAnimationState& animation_state = animation_states[entity_position->id];
            if (position_changed) {
                int current_row = animation_row_for_delta(
                        static_cast<int>(entity_position->x) - previous_x,
                        static_cast<int>(entity_position->y) - previous_y,
                        animation_state.animation_row);
                animation_state.animation_row = current_row;
                animation_state.walk_frame += 2;  // avanza el cuadro de caminata por tile
                animation_state.last_motion_tick = current_tick;
                animation_state.move_start_tick = current_tick;
            }
            animation_state.is_moving =
                    position_changed ||
                    (current_tick - animation_state.last_motion_tick) < MOTION_GRACE_MS;
            actualizarPosVisualEntidad(animation_state, entity_position->x, entity_position->y,
                                       current_tick);

            if (entity_position->id == idCliente) {
                posX = entity_position->x;
                posY = entity_position->y;
                mapaActual_ = entity_position->mapaId;
            } else if (esNueva && entity_position->tipo == 1) {  // criatura nueva en pantalla
                gestorAudio.reproducirEfectoPosicional(
                        "criaturaAparece",
                        distanciaAlJugador(entity_position->x, entity_position->y));
            }
        } else if (auto* cambio_mapa = std::get_if<MensajeCambioMapa>(&mensaje.payload)) {
            // El jugador local cruzo un portal. El server no manda ENTIDAD_DESAPARECIO
            // de las entidades del mapa viejo (solo avisa a los que se quedan), asi que
            // las purgamos nosotros y dejamos solo al jugador local; el snapshot del
            // mapa destino (que llega a continuacion) repuebla la escena.
            mapaActual_ = cambio_mapa->mapaId;
            for (auto it = entidades.begin(); it != entidades.end();) {
                if (it->first == idCliente) {
                    ++it;
                } else {
                    animation_states.erase(it->first);
                    it = entidades.erase(it);
                }
            }
            oroEnSuelo_.clear();
            itemEnSuelo_.clear();
        } else if (auto* entity_disappeared = std::get_if<MensajeEntidadDesaparecio>(&mensaje.payload)) {
            entidades.erase(entity_disappeared->id);
            animation_states.erase(entity_disappeared->id);
        } else if (auto* dead_entity = std::get_if<MensajeMuerteEntidad>(&mensaje.payload)) {
            if (dead_entity->id == idCliente) {
                gestorAudio.reproducirEfecto("muereJugador");
            } else if (const auto it = entidades.find(dead_entity->id); it != entidades.end()) {
                const char* claveMuerte =
                        (it->second.tipo == static_cast<uint8_t>(TipoEntidad::Criatura))
                                ? "matar"
                                : "muerte";
                gestorAudio.reproducirEfectoPosicional(
                        claveMuerte, distanciaAlJugador(it->second.x, it->second.y));
            }
            entidades.erase(dead_entity->id);
            animation_states.erase(dead_entity->id);
            std::cout << "[cliente] entidad muerta: " << dead_entity->id << std::endl;
        } else if (auto* estado = std::get_if<MensajeEstadoPersonaje>(&mensaje.payload)) {
            if (const auto it = entidades.find(idCliente); it != entidades.end()) {
                it->second.estado = estado->estado;
            }
            if (estado->nivel > nivelAnterior) {
                gestorAudio.reproducirEfecto("subirNivel");
                if (nivelAnterior != 0) {
                    agregarLineaChat("Subiste al nivel " + std::to_string(estado->nivel) + ".",
                                     TipoMensajeChat::Sistema);
                }
            }
            const uint8_t meditando = static_cast<uint8_t>(EstadoEntidadProtocolo::Meditando);
            if (estado->estado == meditando && estadoAnterior != meditando) {
                gestorAudio.reproducirEfecto("meditar");
            }
            // Resurreccion: al entrar en Resucitando arranca el aura + barra + sonido del
            // tiempo transcurriendo; al salir (revivido) se cortan. El sonido de resurreccion
            // silencia el resto, por eso se detiene al final (tras los demas chequeos).
            const uint8_t resucitando = static_cast<uint8_t>(EstadoEntidadProtocolo::Resucitando);
            const bool entraResurreccion =
                    estado->estado == resucitando && estadoAnterior != resucitando;
            const bool saleResurreccion =
                    estadoAnterior == resucitando && estado->estado != resucitando;
            if (entraResurreccion) {
                resurreccionActiva_ = true;
                resurreccionInicioTick_ = current_tick;
                resurreccionDuracionMs_ = estado->tiempoResurreccionMs;
                gestorAudio.iniciarResurreccion();
            }
            const bool vivo = (estado->estado == static_cast<uint8_t>(EstadoEntidadProtocolo::Vivo));
            const bool vidaCritica = vivo && estado->vidaMax > 0 &&
                                     (estado->vidaActual * 100 / estado->vidaMax) <= UMBRAL_VIDA_BAJA;
            if (vidaCritica && !vidaBajaAvisada) {
                gestorAudio.reproducirEfecto("vidaBaja");
            }
            // Suba marcada de vida/mana => curacion o pocion (no la regen natural).
            if (vidaAnterior > 0 && estado->vidaActual >= vidaAnterior + UMBRAL_CURACION) {
                gestorAudio.reproducirEfecto("curarVida");
                agregarLineaChat("Recuperaste " + std::to_string(estado->vidaActual - vidaAnterior) +
                                 " de vida.", TipoMensajeChat::Recuperacion);
            }
            if (manaAnterior > 0 && estado->manaActual >= manaAnterior + UMBRAL_CURACION) {
                gestorAudio.reproducirEfecto("curarMana");
                agregarLineaChat("Recuperaste " + std::to_string(estado->manaActual - manaAnterior) +
                                 " de mana.", TipoMensajeChat::Recuperacion);
            }
            // Ganancia de experiencia (el guard evita el mensaje espurio al conectar).
            if (experienciaAnterior > 0 && estado->experiencia > experienciaAnterior) {
                agregarLineaChat("Ganaste " +
                                 std::to_string(estado->experiencia - experienciaAnterior) +
                                 " de experiencia.", TipoMensajeChat::Experiencia);
            }
            // Aumento de oro => juntar monedas (del suelo) o el cheat de oro (F4).
            if (oroAnterior > 0 && estado->oro > oroAnterior) {
                gestorAudio.reproducirEfecto("agarrarMoneda");
            }
            vidaBajaAvisada = vidaCritica;
            vidaAnterior = estado->vidaActual;
            manaAnterior = estado->manaActual;
            experienciaAnterior = estado->experiencia;
            oroAnterior = estado->oro;
            nivelAnterior = estado->nivel;
            estadoAnterior = estado->estado;
            // Stats para el panel derecho.
            estadoJugador_ = EstadoJugador{estado->vidaActual, estado->vidaMax,
                                           estado->manaActual, estado->manaMax, estado->oro,
                                           estado->experiencia, estado->nivel, estado->raza,
                                           estado->clase, estado->expSiguienteNivel};
            std::cout << "[cliente] estado personaje: vida " << estado->vidaActual << "/"
                      << estado->vidaMax << ", mana " << estado->manaActual << "/"
                      << estado->manaMax << ", oro " << estado->oro << ", nivel "
                      << static_cast<int>(estado->nivel) << std::endl;
            // Se detiene al final para que los efectos de este mismo mensaje (ej. curacion al
            // revivir con vida a la mitad) sigan silenciados hasta soltar el sonido del tiempo.
            if (saleResurreccion) {
                resurreccionActiva_ = false;
                gestorAudio.detenerResurreccion();
            }
        } else if (auto* dano_recibido = std::get_if<MensajeDanoRecibido>(&mensaje.payload)) {
            gestorAudio.reproducirEfecto("recibirDanio");
            
            if (const auto it = entidades.find(dano_recibido->idAtacante);
                it != entidades.end() &&
                it->second.tipo == static_cast<uint8_t>(TipoEntidad::Criatura)) {
                gestorAudio.reproducirEfectoPosicional(
                        "criaturaAtacando", distanciaAlJugador(it->second.x, it->second.y));
            }
            if (dano_recibido->esCritico) {
                agregarLineaChat("Te dieron un golpe CRITICO de " +
                                 std::to_string(dano_recibido->cantidad) + " de dano.",
                                 TipoMensajeChat::CriticoRecibido);
            } else {
                agregarLineaChat("Te atacaron y recibiste " +
                                 std::to_string(dano_recibido->cantidad) + " de dano.",
                                 TipoMensajeChat::Ataque);
            }
        } else if (auto* dano_producido = std::get_if<MensajeDanoProducido>(&mensaje.payload)) {
            // El sonido depende del arma con que se golpeo (lo decide el server).
            const char* claveAtaque = "ataqueEspada";
            switch (static_cast<TipoGolpe>(dano_producido->tipoGolpe)) {
                case TipoGolpe::Hacha:     claveAtaque = "ataqueHacha"; break;
                case TipoGolpe::Martillo:  claveAtaque = "ataqueMartillo"; break;
                case TipoGolpe::Disparo:   claveAtaque = "disparoDistancia"; break;
                case TipoGolpe::Hechizo:   claveAtaque = "lanzarHechizo"; break;
                case TipoGolpe::Explosion: claveAtaque = "explosion"; break;
                case TipoGolpe::Espada:    claveAtaque = "ataqueEspada"; break;
            }
            if (const auto it = entidades.find(dano_producido->idObjetivo); it != entidades.end()) {
                gestorAudio.reproducirEfectoPosicional(
                        claveAtaque, distanciaAlJugador(it->second.x, it->second.y));
            } else {
                gestorAudio.reproducirEfecto(claveAtaque);
            }
            if (dano_producido->esCritico) {
                agregarLineaChat("Hiciste un golpe CRITICO de " +
                                 std::to_string(dano_producido->cantidad) + " de dano.",
                                 TipoMensajeChat::CriticoHecho);
            } else {
                agregarLineaChat("Atacaste e hiciste " + std::to_string(dano_producido->cantidad) +
                                 " de dano.");
            }
        } else if (auto* esquive = std::get_if<MensajeEsquive>(&mensaje.payload)) {
            if (const auto it = entidades.find(esquive->idEntidad); it != entidades.end()) {
                gestorAudio.reproducirEfectoPosicional(
                        "esquive", distanciaAlJugador(it->second.x, it->second.y));
            } else {
                gestorAudio.reproducirEfecto("esquive");
            }
            agregarLineaChat("El ataque fue esquivado.");
        } else if (auto* mensaje_chat = std::get_if<MensajeChat>(&mensaje.payload)) {
            TipoMensajeChat tipoChat = TipoMensajeChat::Normal;
            if (mensaje_chat->tipo == static_cast<uint8_t>(CategoriaChat::Privado)) {
                tipoChat = TipoMensajeChat::Privado;
            } else if (mensaje_chat->tipo == static_cast<uint8_t>(CategoriaChat::Sistema)) {
                tipoChat = TipoMensajeChat::Sistema;
            }
            const std::string linea = mensaje_chat->nickOrigen.empty()
                    ? mensaje_chat->mensaje
                    : mensaje_chat->nickOrigen + ": " + mensaje_chat->mensaje;
            agregarLineaChat(linea, tipoChat);
        } else if (auto* error_accion = std::get_if<MensajeErrorAccion>(&mensaje.payload)) {
            // El cooldown de ataque es esperado al golpear seguido: no lo mostramos.
            if (error_accion->codigo == CodigoErrorAccion::COOLDOWN_ATAQUE) {
                continue;
            }
            gestorAudio.reproducirEfecto("accionNoPermitida");
            const std::string texto = MensajesErrorAccion::mensaje(error_accion->codigo);
            agregarLineaChat("* " + texto);
        } else if (auto* resucitado = std::get_if<MensajeResucitado>(&mensaje.payload)) {
            posX = resucitado->x;
            posY = resucitado->y;
            EntityAnimationState& animation_state = animation_states[idCliente];
            animation_state.is_moving = true;
            animation_state.last_motion_tick = current_tick;
            animation_state.vis_x = resucitado->x;
            animation_state.vis_y = resucitado->y;
            animation_state.vis_init = true;
            animation_state.vis_last_tick = current_tick;
            gestorAudio.reproducirEfecto("reviviendo");
            std::cout << "[cliente] resucitado en (" << resucitado->x << ", "
                      << resucitado->y << ")" << std::endl;
        } else if (auto* item_suelo = std::get_if<MensajeItemEnSuelo>(&mensaje.payload)) {
            gestorAudio.reproducirEfectoPosicional(
                    "caerObjeto", distanciaAlJugador(item_suelo->x, item_suelo->y));
            itemEnSuelo_.insert({item_suelo->x, item_suelo->y});
        } else if (auto* oro_suelo = std::get_if<MensajeOroEnSuelo>(&mensaje.payload)) {
            gestorAudio.reproducirEfectoPosicional(
                    "caerObjeto", distanciaAlJugador(oro_suelo->x, oro_suelo->y));
            oroEnSuelo_.insert({oro_suelo->x, oro_suelo->y});
        } else if (auto* inv = std::get_if<MensajeActualizarInventario>(&mensaje.payload)) {
            inventario_ = inv->slots_;
            gestorAudio.reproducirEfecto("agarrarItem");
        } else if (auto* equip = std::get_if<MensajeActualizarEquipamiento>(&mensaje.payload)) {
            equipamiento_ = EquipamientoJugador{equip->arma, equip->baculo, equip->defensa,
                                                equip->casco, equip->escudo};
        } else if (auto* item_desaparecio = std::get_if<MensajeItemDesaparecioSuelo>(&mensaje.payload)) {
            itemEnSuelo_.erase({item_desaparecio->x, item_desaparecio->y});
        } else if (auto* oro_desaparecio = std::get_if<MensajeOroDesaparecioSuelo>(&mensaje.payload)) {
            oroEnSuelo_.erase({oro_desaparecio->x, oro_desaparecio->y});
        } else if (auto* recibir_lista_items = std::get_if<MensajeListaItems>(&mensaje.payload)) {
            // El stock se muestra en el panel de comercio; no ensuciamos el chat con ids.
            stockNpc_ = recibir_lista_items->ids;
        } else if (auto* banco = std::get_if<MensajeContenidoBanco>(&mensaje.payload)) {
            bancoItems_ = banco->items;
            bancoOro_ = banco->oroBanco;
            bancoRecibido_ = true;
        } else if (auto* hech = std::get_if<MensajeListaHechizos>(&mensaje.payload)) {
            hechizosConocidos_ = hech->ids;
        } else if (auto* fx = std::get_if<MensajeFxHechizo>(&mensaje.payload)) {
            fxPendientes_.emplace_back(fx->idHechizo, fx->idObjetivo);
        } else if (auto* pr = std::get_if<MensajeProyectil>(&mensaje.payload)) {
            proyectilesPendientes_.emplace_back(pr->idOrigen, pr->idDestino);
        }
    }

    for (auto& [entity_id, animation_state] : animation_states) {
        const bool client_position_changed =
                entity_id == idCliente && (posX != previous_pos_x || posY != previous_pos_y);
        const auto entity_it = entidades.find(entity_id);
        if (entity_it != entidades.end()) {
            actualizarPosVisualEntidad(animation_state, entity_it->second.x, entity_it->second.y,
                                       current_tick);
        }
        if (!client_position_changed) {
            animation_state.is_moving =
                    (current_tick - animation_state.last_motion_tick) < MOTION_GRACE_MS;
        }
    }

    
    const auto miAnimacion = animation_states.find(idCliente);
    if (miAnimacion != animation_states.end() && miAnimacion->second.is_moving) {
        gestorAudio.reproducirPasos();
    } else {
        gestorAudio.detenerPasos();
    }
}

void ObjectGameWorld::notify_move_requested(const uint32_t current_tick) {
    EntityAnimationState& animation_state = animation_states[idCliente];
    animation_state.is_moving = true;
    animation_state.last_motion_tick = current_tick;
}

const std::unordered_map<uint16_t, EntidadRenderizable>& ObjectGameWorld::entities() const {
    return entidades;
}

uint16_t ObjectGameWorld::client_id() const {
    return idCliente;
}

int ObjectGameWorld::player_x() const {
    return posX;
}

int ObjectGameWorld::player_y() const {
    return posY;
}

bool ObjectGameWorld::player_is_moving() const {
    return entity_is_moving(idCliente);
}

bool ObjectGameWorld::entity_is_moving(const uint16_t entity_id) const {
    const auto it = animation_states.find(entity_id);
    return it != animation_states.end() && it->second.is_moving;
}

int ObjectGameWorld::entity_animation_row(const uint16_t entity_id) const {
    const auto it = animation_states.find(entity_id);
    return (it != animation_states.end()) ? it->second.animation_row : 0;
}

int ObjectGameWorld::entity_walk_frame(const uint16_t entity_id) const {
    const auto it = animation_states.find(entity_id);
    return (it != animation_states.end()) ? it->second.walk_frame : 0;
}

// agrego dos metodos para obtener la pos_visual tanto en x como en y de los jugadores
// para usarla en la camara
double ObjectGameWorld::entity_visual_x(const uint16_t entity_id) const {
    const auto anim_it = animation_states.find(entity_id);
    if (anim_it != animation_states.end() && anim_it->second.vis_init) {
        return anim_it->second.vis_x;
    }
    const auto entity_it = entidades.find(entity_id);
    return (entity_it != entidades.end()) ? entity_it->second.x : 0.0;
}

double ObjectGameWorld::entity_visual_y(const uint16_t entity_id) const {
    const auto anim_it = animation_states.find(entity_id);
    if (anim_it != animation_states.end() && anim_it->second.vis_init) {
        return anim_it->second.vis_y;
    }
    const auto entity_it = entidades.find(entity_id);
    return (entity_it != entidades.end()) ? entity_it->second.y : 0.0;
}

void ObjectGameWorld::agregarLineaChat(const std::string& linea, TipoMensajeChat tipo) {
    historialChatReciente.emplace_back(linea, tipo);
    while (historialChatReciente.size() > maxLineasChat) {
        historialChatReciente.pop_front();
    }
}

void ObjectGameWorld::mensajeLocal(const std::string& linea, TipoMensajeChat tipo) {
    agregarLineaChat(linea, tipo);
}

const std::deque<LineaChat>& ObjectGameWorld::historialChat() const {
    return historialChatReciente;
}

void ObjectGameWorld::setMaxLineasChat(const size_t maximo) {
    if (maximo > 0) {
        maxLineasChat = maximo;
    }
}

const std::vector<uint16_t>& ObjectGameWorld::inventario() const {
    return inventario_;
}

const EquipamientoJugador& ObjectGameWorld::equipamiento() const {
    return equipamiento_;
}

const EstadoJugador& ObjectGameWorld::estadoJugador() const {
    return estadoJugador_;
}

const std::vector<uint16_t>& ObjectGameWorld::stockNpc() const {
    return stockNpc_;
}

const std::vector<uint16_t>& ObjectGameWorld::bancoItems() const {
    return bancoItems_;
}

uint32_t ObjectGameWorld::bancoOro() const {
    return bancoOro_;
}

bool ObjectGameWorld::bancoRecibido() const {
    return bancoRecibido_;
}

void ObjectGameWorld::cerrarBanco() {
    bancoRecibido_ = false;
}

void ObjectGameWorld::abrirTienda(bool esSacerdote) {
    tiendaAbierta_ = true;
    tiendaEsSacerdote_ = esSacerdote;
}

void ObjectGameWorld::cerrarTienda() {
    tiendaAbierta_ = false;
}

bool ObjectGameWorld::tiendaAbierta() const {
    return tiendaAbierta_;
}

bool ObjectGameWorld::tiendaEsSacerdote() const {
    return tiendaEsSacerdote_;
}

const std::vector<uint16_t>& ObjectGameWorld::hechizosConocidos() const {
    return hechizosConocidos_;
}

float ObjectGameWorld::fraccionResurreccionRestante(uint32_t tick) const {
    if (!resurreccionActiva_ || resurreccionDuracionMs_ == 0) {
        return 0.0f;
    }
    const uint32_t transcurrido = tick - resurreccionInicioTick_;
    const float restante = 1.0f - static_cast<float>(transcurrido) / resurreccionDuracionMs_;
    return std::clamp(restante, 0.0f, 1.0f);
}

std::vector<std::pair<uint16_t, uint16_t>> ObjectGameWorld::drenarFx() {
    std::vector<std::pair<uint16_t, uint16_t>> fx;
    fx.swap(fxPendientes_);
    return fx;
}

std::vector<std::pair<uint16_t, uint16_t>> ObjectGameWorld::drenarProyectiles() {
    std::vector<std::pair<uint16_t, uint16_t>> pr;
    pr.swap(proyectilesPendientes_);
    return pr;
}
