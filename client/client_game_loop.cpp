
#include "client_game_loop.h"

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <utility>

#include <SDL.h>

#include "audio/gestor_audio.h"

#ifndef CLIENT_GAME_CONFIG_PATH
#define CLIENT_GAME_CONFIG_PATH "config/game_config.toml"
#endif

#ifndef CLIENT_ASSETS_DIR
#define CLIENT_ASSETS_DIR "client/assets"
#endif

ClientGameLoop::ClientGameLoop(Queue<MensajeServidor>& server_messages,
                               ClientBusiness& business,
                               const uint16_t idCliente,
                               const ConfigCliente& config):
    server_messages(server_messages),
    business(business),
    object_state(idCliente),
    config(config),
    catalogo(CLIENT_GAME_CONFIG_PATH),
    parser(catalogo.mapaClaveAId()),
    is_running(false) {}

ClientGameLoop::~ClientGameLoop() = default;

void ClientGameLoop::init(const char* title,
                          const int xpos,
                          const int ypos,
                          const int width,
                          const int height,
                          const bool fullscreen) {
    const auto aColor = [](const std::vector<int>& rgb, const SDL_Color& porDefecto) -> SDL_Color {
        if (rgb.size() != 3) {
            return porDefecto;
        }
        return SDL_Color{static_cast<uint8_t>(rgb[0]), static_cast<uint8_t>(rgb[1]),
                         static_cast<uint8_t>(rgb[2]), 255};
    };
    ConfigChatRender chatCfg;
    chatCfg.fuenteRuta = config.fuenteRuta;
    chatCfg.fuenteTam = config.fuenteTam;
    chatCfg.fondoRuta = config.fondoChatRuta;
    chatCfg.panelX = config.chatPanelX;
    chatCfg.panelY = config.chatPanelY;
    chatCfg.panelAlto = config.chatPanelAlto;
    chatCfg.colorTexto = aColor(config.chatColorTexto, chatCfg.colorTexto);
    chatCfg.colorInput = aColor(config.chatColorInput, chatCfg.colorInput);

    ConfigPanelRender panelCfg;
    panelCfg.ancho = config.panelAncho;
    panelCfg.iconDir = config.panelIconDir;
    panelCfg.fondoCuero = config.panelFondoCuero;
    panelCfg.colorTexto = aColor(config.panelColorTexto, panelCfg.colorTexto);
    panelCfg.colorTitulo = aColor(config.panelColorTitulo, panelCfg.colorTitulo);
    panelCfg.bancoBovedaX = config.bancoBovedaX;
    panelCfg.bancoBovedaY = config.bancoBovedaY;
    panelCfg.bancoInvX = config.bancoInvX;
    panelCfg.bancoInvY = config.bancoInvY;
    panelCfg.bancoSlot = config.bancoSlot;
    panelCfg.bancoGap = config.bancoGap;
    panelCfg.bancoCols = config.bancoCols;

    object_renderer.init(title, xpos, ypos, width, height, fullscreen, config.vsync,
                         config.fpsMax, chatCfg, panelCfg, &catalogo);
    object_state.setMaxLineasChat(static_cast<size_t>(config.chatMaxLineas));
    handler.set_window_dimensions(width, height);
    handler.setChatPanel(config.chatPanelX, config.chatPanelY, config.chatPanelAlto);
    handler.setAnchoPanel(config.panelAncho);
    handler.setIdCliente(object_state.client_id());

    const std::string resourcesRoot = std::string(CLIENT_ASSETS_DIR) + "/../resources";
    gestorAudio = std::make_unique<GestorAudio>(resourcesRoot + "/config/sonidos.toml",
                                                resourcesRoot);
    gestorAudio->reproducirMusica("campo");

    const uint32_t frame_target_ms = 1000u / static_cast<uint32_t>(config.fpsMax);
    int it = 0;
    uint32_t tick = SDL_GetTicks();
    is_running = true;
    while (is_running) {
        handleEvents();
        update(it);
        render();
        const uint32_t tick2 = SDL_GetTicks();
        int rest = static_cast<int>(frame_target_ms) - static_cast<int>(tick2 - tick);
        if (rest < 0) {
            const int behind = -rest;
            rest = frame_target_ms - (behind % frame_target_ms);
            const int lost = behind + rest;
            tick += lost;
            it += lost / frame_target_ms;
        } else {
            SDL_Delay(rest);
        }
        tick +=  frame_target_ms;
        it ++;
    }
}

void ClientGameLoop::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Si el banco esta abierto, es modal: todos los eventos van a su manejo.
        if (object_state.bancoRecibido()) {
            manejarEventoBanco(event);
            continue;
        }
        // Rueda del mouse: scrollea la lista de comercio del panel.
        if (event.type == SDL_MOUSEWHEEL) {
            const int total = static_cast<int>(object_state.stockNpc().size());
            scrollComercio -= event.wheel.y;
            if (scrollComercio < 0) {
                scrollComercio = 0;
            }
            if (scrollComercio > std::max(0, total - 1)) {
                scrollComercio = std::max(0, total - 1);
            }
            continue;
        }
        // Click sobre el panel derecho: lo maneja el loop (tiene renderer + estado + catalogo).
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.x >= config.ancho - config.panelAncho) {
            manejarClickPanel(event.button.x, event.button.y);
            continue;
        }
        const ResultadoInput resultado = handler.handle_event(event, object_state.entities());
        if (resultado.comando) {
            despacharComando(*resultado.comando, SDL_GetTicks());
        }
        if (resultado.lineaChat) {
            procesarLineaChat(*resultado.lineaChat, SDL_GetTicks());
        }
    }

    if (handler.should_quit()) {
        is_running = false;
    }
}

void ClientGameLoop::procesarLineaChat(const std::string& linea, const uint32_t current_tick) {
    const ResultadoParseo resultado = parser.parsear(linea, handler.objetivoSeleccionado());
    if (resultado.comando) {
        despacharComando(*resultado.comando, current_tick);
    } else if (!resultado.error.empty()) {
        std::cerr << "[chat] " << resultado.error << std::endl;
    }
}

void ClientGameLoop::despacharComando(const ComandoJugador& command, const uint32_t current_tick) {
    if (auto action = animation_action_for_command(command)) {
        object_animation.on_action(*action);
        object_state.notify_move_requested(current_tick);
    }
    reproducirSonidoDeComando(command);
    business.save_command(command);
}

void ClientGameLoop::manejarClickPanel(const int x, const int y) {
    const uint32_t tick = SDL_GetTicks();
    const std::optional<uint16_t> objetivo = handler.objetivoSeleccionado();
    const std::vector<uint16_t>& items = object_state.inventario();

    // Pestañas del marco: cambian entre inventario y hechizos.
    if (object_renderer.clickTabHechizos(x, y)) {
        pestanaHechizos = true;
        return;
    }
    if (object_renderer.clickTabInventario(x, y)) {
        pestanaHechizos = false;
        return;
    }

    // 1) Boton Vender: vende el item seleccionado (requiere comerciante seleccionado).
    if (object_renderer.clickEnBotonVender(x, y)) {
        if (slotInvSeleccionado >= 0 && objetivo &&
            slotInvSeleccionado < static_cast<int>(items.size()) &&
            items[slotInvSeleccionado] != 0) {
            despacharComando({Opcode::VENDER,
                              ComandoVender{static_cast<uint8_t>(slotInvSeleccionado), *objetivo}},
                             tick);
            slotInvSeleccionado = -1;  // se limpia la seleccion tras vender
        }
        return;
    }

    // 1b) Boton Equipar: equipa el item seleccionado (no encontramos un boton que diga equipar, asi que usamos uno que dice CONSTRUIR).
    if (object_renderer.clickEnBotonEquipar(x, y)) {
        if (slotInvSeleccionado >= 0 &&
            slotInvSeleccionado < static_cast<int>(items.size()) &&
            items[slotInvSeleccionado] != 0) {
            despacharComando({Opcode::EQUIPAR,
                              ComandoEquipar{static_cast<uint8_t>(slotInvSeleccionado)}}, tick);
            slotInvSeleccionado = -1;
        }
        return;
    }

    // Boton Usar: usa el item seleccionado
    if (object_renderer.clickEnBotonUsar(x, y)) {
        if (slotInvSeleccionado >= 0 &&
            slotInvSeleccionado < static_cast<int>(items.size()) &&
            items[slotInvSeleccionado] != 0) {
            despacharComando({Opcode::USAR,
                              ComandoUsar{static_cast<uint8_t>(slotInvSeleccionado)}}, tick);
            slotInvSeleccionado = -1;
        }
        return;
    }

    // Boton Curar: pide curacion al objetivo. Si no es un sacerdote (o no hay objetivo)"accion no permitida" y suena.
    if (object_renderer.clickEnBotonCurar(x, y)) {
        despacharComando({Opcode::CURAR, ComandoCurar{objetivo.value_or(static_cast<uint16_t>(0))}},
                         tick);
        return;
    }

    // Pestaña HECHIZOS: lanzar (los listados son conocidos).
    if (const uint16_t idHechizo = object_renderer.hechizoClickeado(x, y); idHechizo != 0) {
        despacharComando({Opcode::LANZAR_HECHIZO,
                          ComandoLanzarHechizo{idHechizo, objetivo.value_or(0)}}, tick);
        return;
    }
    // Lista de hechizos del sacerdote: comprar.
    if (const uint16_t idHechizo = object_renderer.hechizoVentaClickeado(x, y); idHechizo != 0) {
        despacharComando({Opcode::COMPRAR_HECHIZO,
                          ComandoComprarHechizo{idHechizo, objetivo.value_or(0)}}, tick);
        return;
    }

    // 2) Click en el stock del comerciante -> comprar (requiere NPC seleccionado).
    const int s = object_renderer.slotStockClickeado(x, y);
    if (s >= 0) {
        const std::vector<uint16_t>& stock = object_state.stockNpc();
        const int idx = scrollComercio + s;  // el scroll desplaza la lista visible
        if (objetivo && idx < static_cast<int>(stock.size())) {
            despacharComando({Opcode::COMPRAR, ComandoComprar{stock[idx], *objetivo}}, tick);
        }
        return;
    }

    // 3) Click en el inventario -> seleccionar (zoom). El vender es por el boton.
    const int inv = object_renderer.slotInventarioClickeado(x, y);
    if (inv >= 0) {
        if (inv < static_cast<int>(items.size()) && items[inv] != 0) {
            slotInvSeleccionado = (slotInvSeleccionado == inv) ? -1 : inv;  // toggle
        }
    }
}

void ClientGameLoop::manejarEventoBanco(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        is_running = false;
        return;
    }
    const uint32_t tick = SDL_GetTicks();
    const std::optional<uint16_t> banquero = handler.objetivoSeleccionado();

    // Caja de monto activa: capturamos digitos por teclado (sin SDL_TextInput).
    if (bancoMontoActivo && event.type == SDL_KEYDOWN) {
        const SDL_Keycode k = event.key.keysym.sym;
        if (k >= SDLK_0 && k <= SDLK_9 && bancoMonto.size() < 9) {
            bancoMonto += static_cast<char>('0' + (k - SDLK_0));
        } else if (k >= SDLK_KP_1 && k <= SDLK_KP_9 && bancoMonto.size() < 9) {
            bancoMonto += static_cast<char>('1' + (k - SDLK_KP_1));
        } else if (k == SDLK_KP_0 && bancoMonto.size() < 9) {
            bancoMonto += '0';
        } else if (k == SDLK_BACKSPACE && !bancoMonto.empty()) {
            bancoMonto.pop_back();
        } else if (k == SDLK_RETURN || k == SDLK_KP_ENTER || k == SDLK_ESCAPE) {
            bancoMontoActivo = false;
        }
        return;
    }

    if (event.type != SDL_MOUSEBUTTONDOWN) {
        return;
    }
    const int x = event.button.x;
    const int y = event.button.y;

    if (object_renderer.clickBancoCerrar(x, y)) {
        object_state.cerrarBanco();
        bancoSelBoveda = bancoSelInv = -1;
        bancoMonto.clear();
        bancoMontoActivo = false;
        return;
    }
    if (object_renderer.clickBancoCajaMonto(x, y)) {
        bancoMontoActivo = true;
        return;
    }
    bancoMontoActivo = false;

    if (const int bv = object_renderer.bancoBovedaClickeada(x, y); bv >= 0) {
        bancoSelBoveda = (bancoSelBoveda == bv) ? -1 : bv;
        return;
    }
    if (const int iv = object_renderer.bancoInvClickeado(x, y); iv >= 0) {
        bancoSelInv = (bancoSelInv == iv) ? -1 : iv;
        return;
    }

    const std::vector<uint16_t>& boveda = object_state.bancoItems();
    const std::vector<uint16_t>& inv = object_state.inventario();

    if (object_renderer.clickBancoDepositar(x, y)) {
        if (banquero && bancoSelInv >= 0 && bancoSelInv < static_cast<int>(inv.size()) &&
            inv[bancoSelInv] != 0) {
            despacharComando({Opcode::DEPOSITAR_ITEM,
                              ComandoDepositarItem{static_cast<uint8_t>(bancoSelInv), *banquero}},
                             tick);
            bancoSelInv = -1;
        }
        return;
    }
    if (object_renderer.clickBancoRetirar(x, y)) {
        if (banquero && bancoSelBoveda >= 0 && bancoSelBoveda < static_cast<int>(boveda.size())) {
            despacharComando({Opcode::RETIRAR_ITEM,
                              ComandoRetirarItem{boveda[bancoSelBoveda], *banquero}}, tick);
            bancoSelBoveda = -1;
        }
        return;
    }

    const uint32_t monto =
            bancoMonto.empty() ? 0u : static_cast<uint32_t>(std::strtoul(bancoMonto.c_str(),
                                                                         nullptr, 10));
    if (object_renderer.clickBancoDepositarOro(x, y)) {
        if (banquero && monto > 0) {
            despacharComando({Opcode::DEPOSITAR_ORO, ComandoDepositarOro{monto, *banquero}}, tick);
        }
        bancoMonto.clear();
        return;
    }
    if (object_renderer.clickBancoRetirarOro(x, y)) {
        if (banquero && monto > 0) {
            despacharComando({Opcode::RETIRAR_ORO, ComandoRetirarOro{monto, *banquero}}, tick);
        }
        bancoMonto.clear();
        return;
    }
}

void ClientGameLoop::reproducirSonidoDeComando(const ComandoJugador& command) {
    if (!gestorAudio) {
        return;
    }
    switch (command.opcode) {
        case Opcode::COMPRAR:
            gestorAudio->reproducirEfecto("comercianteComprar");
            break;
        case Opcode::VENDER:
            gestorAudio->reproducirEfecto("comercianteVender");
            break;
        case Opcode::LISTAR:
            gestorAudio->reproducirEfecto("comercianteBienvenida");
            break;
        case Opcode::CURAR:
            gestorAudio->reproducirEfecto("sacerdoteInteraccion");
            break;
        case Opcode::DEPOSITAR_ITEM:
        case Opcode::DEPOSITAR_ORO:
        case Opcode::RETIRAR_ITEM:
        case Opcode::RETIRAR_ORO:
            gestorAudio->reproducirEfecto("banqueroBienvenida");
            break;
        default:
            break;
    }
}

void ClientGameLoop::update(const int it) {
    const uint32_t current_tick = SDL_GetTicks();
    object_state.upload_server_msg(server_messages, current_tick, *gestorAudio);
    object_renderer.update_animation(it, object_state, object_animation);
}

void ClientGameLoop::render() {
    // Solo lo dinamico: la config del chat ya la tiene el renderer desde init().
    EstadoChatRender chat;
    chat.activo = handler.chatActivo();
    chat.entrada = handler.bufferChat();
    const std::deque<std::string>& historial = object_state.historialChat();
    chat.historial.assign(historial.begin(), historial.end());

    EstadoPanelRender panel;
    panel.inventario = object_state.inventario();
    panel.equip = object_state.equipamiento();
    panel.stats = object_state.estadoJugador();
    panel.stock = object_state.stockNpc();
    panel.seleccionInventario = slotInvSeleccionado;
    panel.scrollStock = scrollComercio;
    panel.hechizosConocidos = object_state.hechizosConocidos();
    panel.mostrarHechizos = pestanaHechizos;
    const std::optional<uint16_t> objetivoPanel = handler.objetivoSeleccionado();
    panel.sacerdoteSeleccionado =
            objetivoPanel.has_value() && object_renderer.esSacerdote(*objetivoPanel);

    EstadoBancoRender banco;
    banco.abierto = object_state.bancoRecibido();
    if (banco.abierto) {
        banco.boveda = object_state.bancoItems();
        banco.inventario = object_state.inventario();
        banco.oroBanco = object_state.bancoOro();
        banco.oroJugador = object_state.estadoJugador().oro;
        banco.selBoveda = bancoSelBoveda;
        banco.selInventario = bancoSelInv;
        banco.monto = bancoMonto;
        banco.montoActivo = bancoMontoActivo;
    }

    object_renderer.render(object_state, object_animation, chat, panel, banco);
}

void ClientGameLoop::clean() {}

bool ClientGameLoop::isRunning() const {
    return is_running;
}

std::optional<GameAction> ClientGameLoop::animation_action_for_command(
        const ComandoJugador& command) const {
    if (command.opcode != Opcode::EMPEZAR_MOVER) {
        return std::nullopt;
    }

    const auto* move = std::get_if<ComandoEmpezarMover>(&command.payload);
    if (move == nullptr) {
        return std::nullopt;
    }

    switch (move->direccion) {
        case 0:
            return GameAction::MoveUp;
        case 1:
            return GameAction::MoveDown;
        case 2:
            return GameAction::MoveLeft;
        case 3:
            return GameAction::MoveRight;
        default:
            return std::nullopt;
    }
}
