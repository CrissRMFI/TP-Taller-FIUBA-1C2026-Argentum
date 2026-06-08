#include "client_input_handler.h"

#include <algorithm>
#include <cmath>

#include "../../common/protocolo/tipo_entidad.h"

#define MAX_CHARS_CHAT 200

ClientInputHandler::ClientInputHandler():
        quit_requested(false), window_width(0), window_height(0), idCliente(0),
        chat_activo(false) {}

ClientInputHandler::~ClientInputHandler() = default;

std::optional<ComandoJugador> ClientInputHandler::handle_keyboard(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE:
            quit_requested = true;
            return std::nullopt;
        case SDLK_m:
            return ComandoJugador{Opcode::MEDITAR, ComandoMeditar{}};
        case SDLK_r:
            return ComandoJugador{Opcode::RESUCITAR, ComandoResucitar{}};
        // Cheats de prueba (teclas): F1 vida infinita, F2 mana infinito,
        // F3 morir al instante, F4 +1000 de oro.
        case SDLK_F1:
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::VidaInfinita)}};
        case SDLK_F2:
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::ManaInfinito)}};
        case SDLK_F3:
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::MorirAuto)}};
        case SDLK_F4:  // cheat de prueba: +1000 de oro
            return ComandoJugador{
                    Opcode::CHEAT,
                    ComandoCheat{static_cast<uint8_t>(TipoCheat::DarOro)}};
        default: return std::nullopt;
    }
}

void ClientInputHandler::abrir_chat() {
    chat_activo = true;
    SDL_StartTextInput();
}

void ClientInputHandler::cerrar_chat() {
    chat_activo = false;
    SDL_StopTextInput();
}

ResultadoInput ClientInputHandler::manejar_texto_chat(const SDL_Event& event) {
    ResultadoInput resultado;

    if (event.type == SDL_TEXTINPUT) {
        if (chat_buffer.size() < MAX_CHARS_CHAT) {
            chat_buffer += event.text.text;
        }
        return resultado;
    }

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER: {
                if (!chat_buffer.empty()) {
                    resultado.lineaChat = chat_buffer;
                    chat_buffer.clear();
                }
                break;
            }
            case SDLK_ESCAPE:
                cerrar_chat();
                break;
            case SDLK_BACKSPACE:
                if (!chat_buffer.empty()) {
                    chat_buffer.pop_back();
                }
                break;
            default:
                break;
        }
    }

    return resultado;
}

bool ClientInputHandler::click_en_chat(const int x, const int y) const {
    // El ancho de la caja es la mitad de la ventana (igual que el renderer).
    const int ancho = window_width / 2;
    return x >= chat_panel_x && x < chat_panel_x + ancho &&
           y >= chat_panel_y && y < chat_panel_y + chat_panel_alto;
}

int ClientInputHandler::ancho_juego() const {
    const int gw = window_width - ancho_panel;
    return (gw > 0) ? gw : window_width;
}

ResultadoInput ClientInputHandler::handle_event(
        const SDL_Event& event,
        const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {
    ResultadoInput resultado;

    if (event.type == SDL_QUIT) {
        quit_requested = true;
        return resultado;
    }

    if (chat_activo) {
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            !click_en_chat(event.button.x, event.button.y)) {
            cerrar_chat();
            resultado.comando = handle_mouse_click(event.button.x, event.button.y, entidades);
            return resultado;
        }
        return manejar_texto_chat(event);
    }

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        const SDL_Keycode key = event.key.keysym.sym;
        // Tecla de direccion presionada: empezar a moverse (el server avanza solo).
        if (auto direccion = direccion_de_tecla(key)) {
            resultado.comando =
                    ComandoJugador{Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{*direccion}};
            return resultado;
        }
        resultado.comando = handle_keyboard(key);
        return resultado;
    }
    if (event.type == SDL_KEYUP) {
        // Tecla de direccion soltada: detener el movimiento.
        if (direccion_de_tecla(event.key.keysym.sym)) {
            resultado.comando = ComandoJugador{Opcode::DETENER_MOVER, ComandoDetenerMover{}};
        }
        return resultado;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (click_en_chat(event.button.x, event.button.y)) {
            abrir_chat();
            resultado.comando = ComandoJugador{Opcode::DETENER_MOVER, ComandoDetenerMover{}};
            return resultado;
        }
        resultado.comando = handle_mouse_click(event.button.x, event.button.y, entidades);
        resultado.clickVacio = !resultado.comando.has_value();  // click en el juego sin objetivo
        return resultado;
    }
    return resultado;
}

std::optional<uint8_t> ClientInputHandler::direccion_de_tecla(SDL_Keycode key) const {
    switch (key) {
        case SDLK_UP:    case SDLK_w: return 0;  // Norte
        case SDLK_DOWN:  case SDLK_s: return 1;  // Sur
        case SDLK_LEFT:  case SDLK_a: return 2;  // Oeste
        case SDLK_RIGHT: case SDLK_d: return 3;  // Este
        default: return std::nullopt;
    }
}

std::optional<ComandoJugador> ClientInputHandler::handle_mouse_click(
        int x, int y,
        const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) {
    const uint16_t idObjetivo = buscar_id_objetivo(x, y, entidades);
    if (idObjetivo == 0) {
        return std::nullopt;
    }

    // El click siempre selecciona a la entidad (para los comandos de chat dirigidos a un objetivo: comprar, curar, depositar, etc.).
    objetivo_seleccionado = idObjetivo;

    const auto it = entidades.find(idObjetivo);
    const bool esNpc =
            it != entidades.end() && it->second.tipo == static_cast<uint8_t>(TipoEntidad::Npc);
    if (esNpc) {
        return ComandoJugador{Opcode::LISTAR, ComandoListar{idObjetivo}};
    }
    return ComandoJugador{Opcode::ATACAR, ComandoAtacar{idObjetivo}};
}

uint16_t ClientInputHandler::buscar_id_objetivo(
        int x, int y,
        const std::unordered_map<uint16_t, EntidadRenderizable>& entidades) const {
    if (window_width <= 0 || window_height <= 0) {
        return 0;
    }

    uint16_t idObjetivo = 0;
    const int alto_juego = (window_height - chat_panel_alto > 0) ? window_height - chat_panel_alto : window_height;
    const float tamanoCeldaX = static_cast<float>(ancho_juego()) / 100.0f;
    const float tamanoCeldaY = static_cast<float>(alto_juego) / 100.0f;
    const float radioJusto =
            std::sqrt(tamanoCeldaX * tamanoCeldaX + tamanoCeldaY * tamanoCeldaY) / 2.0f;
    const float radioSeleccion = std::max(25.0f, radioJusto);
    float mejorDistancia = radioSeleccion + 1.0f;

    for (const auto& [id, entidad] : entidades) {
        if (id == idCliente) {
            continue;
        }

        const float entidadX = (static_cast<float>(entidad.x) + 0.5f) * tamanoCeldaX;
        const float entidadY =
                static_cast<float>(chat_panel_alto) + (static_cast<float>(entidad.y) + 0.5f) * tamanoCeldaY;
        const float dx = entidadX - static_cast<float>(x);
        const float dy = entidadY - static_cast<float>(y);
        const float distancia = std::sqrt(dx * dx + dy * dy);

        if (distancia <= radioSeleccion && distancia < mejorDistancia) {
            mejorDistancia = distancia;
            idObjetivo = id;
        }
    }

    return idObjetivo;
}

bool ClientInputHandler::should_quit() const { return quit_requested; }
