
#include "client_game_loop.h"
#include "registro_cliente.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <utility>

#include <SDL.h>

#include "audio/gestor_audio.h"
#include "../common/game/constant_rate_loop.h"

#ifndef CLIENT_GAME_CONFIG_PATH
#define CLIENT_GAME_CONFIG_PATH "config/game_config.toml"
#endif

#ifndef CLIENT_INTERFACE_DIR
#define CLIENT_INTERFACE_DIR "client/interface"
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
    chatCfg.colorAtaque = aColor(config.chatColorAtaque, chatCfg.colorAtaque);
    chatCfg.colorHechizo = aColor(config.chatColorHechizo, chatCfg.colorHechizo);
    chatCfg.colorSistema = aColor(config.chatColorSistema, chatCfg.colorSistema);
    chatCfg.colorExperiencia = aColor(config.chatColorExperiencia, chatCfg.colorExperiencia);
    chatCfg.colorRecuperacion = aColor(config.chatColorRecuperacion, chatCfg.colorRecuperacion);
    chatCfg.colorClan = aColor(config.chatColorClan, chatCfg.colorClan);
    chatCfg.colorCriticoHecho = aColor(config.chatColorCriticoHecho, chatCfg.colorCriticoHecho);
    chatCfg.colorCriticoRecibido =
            aColor(config.chatColorCriticoRecibido, chatCfg.colorCriticoRecibido);

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
                         config.fpsMax, chatCfg, panelCfg, &catalogo, config.camara,
                         config.intervaloMovimientoMs);
    object_state.setMaxLineasChat(static_cast<size_t>(config.chatMaxLineas));
    handler.set_window_dimensions(width, height);
    handler.setChatPanel(config.chatPanelX, config.chatPanelY, config.chatPanelAlto);
    handler.setAnchoPanel(config.panelAncho);
    handler.setIdCliente(object_state.client_id());
    handler.setMapaDimensiones(object_renderer.anchoMapa(), object_renderer.altoMapa());
    handler.setRadioSeleccion(static_cast<float>(config.seleccionRadioPx));

    const std::string resourcesRoot = std::string(CLIENT_INTERFACE_DIR) + "/../resources";
    gestorAudio = std::make_unique<GestorAudio>(resourcesRoot + "/config/sonidos.toml",
                                                resourcesRoot);
    gestorAudio->reproducirMusica("campo");
    mapaAnteriorAudio = object_renderer.getMapaPrincipal();

    is_running = true;
    // tick -> iteracion dentro del loop --> actualmente tick = 1000/60 = 16 ms
    ConstantRateLoop loop(std::chrono::milliseconds(1000u / static_cast<uint32_t>(config.fpsMax)));
    // el constantRateLoop recibe una funcion por paramtro que
    // calcula el ultimo tick logico alcanzado  en esta vuelta
    loop.run([&](const uint32_t ticks_elapsed, const uint64_t tick_number) {

        const uint64_t current_tick = tick_number + ticks_elapsed - 1;
        handleEvents();
        if (!is_running) {
            loop.stop();
            return;
        }
        update(static_cast<int>(current_tick));
        render();
        if (servidorCaido_) {
        const SDL_MessageBoxButtonData boton = {
                SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
                0, "Aceptar"};
        const SDL_MessageBoxData datos = {
                SDL_MESSAGEBOX_ERROR, nullptr, "Argentum",
                "Se cayo el servidor. El cliente se cerrara.", 1, &boton, nullptr};
        int boton_id = 0;
        SDL_ShowMessageBox(&datos, &boton_id);
    }
    });
}

void ClientGameLoop::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // El banco y la tienda son modales: mientras esten abiertos, todos los eventos van a su manejo.
        if (object_state.bancoRecibido()) {
            manejarEventoBanco(event);
            continue;
        }
        if (object_state.tiendaAbierta()) {
            manejarEventoTienda(event);
            continue;
        }
        // Zoom de la camara con Ctrl + / Ctrl - (no mientras se escribe en el chat).
        if (event.type == SDL_KEYDOWN && !handler.chatActivo() &&
            (SDL_GetModState() & KMOD_CTRL)) {
            const SDL_Keycode k = event.key.keysym.sym;
            if (k == SDLK_PLUS || k == SDLK_EQUALS || k == SDLK_KP_PLUS) {
                object_renderer.zoomIn();
                continue;
            }
            if (k == SDLK_MINUS || k == SDLK_KP_MINUS) {
                object_renderer.zoomOut();
                continue;
            }
        }
        // Click sobre el panel derecho: lo maneja el loop (tiene renderer + estado + catalogo).
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.x >= object_renderer.bordeIzquierdoPanel()) {
            manejarClickPanel(event.button.x, event.button.y);
            continue;
        }
        const ResultadoInput resultado = handler.handle_event(event, object_state.entities());
        if (resultado.comando) {
            despacharComando(*resultado.comando, SDL_GetTicks());
        }
        if (resultado.clickVacio) {
            object_state.mensajeLocal("No hay nada aqui.", TipoMensajeChat::Sistema);
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
        RegistroCliente::error("[chat] " + resultado.error);
    }
}

void ClientGameLoop::despacharComando(const ComandoJugador& command, const uint32_t current_tick) {
    if (auto action = animation_action_for_command(command)) {
        object_animation.on_action(*action);
        object_state.notify_move_requested(current_tick);
    }
    reproducirSonidoDeComando(command);

    if (command.opcode == Opcode::LISTAR) {
        const std::optional<uint16_t> obj = handler.objetivoSeleccionado();
        if (obj.has_value()) {
            if (object_renderer.esComerciante(*obj)) {
                tiendaSelOferta = tiendaSelInv = -1;
                object_state.abrirTienda(false);
            } else if (object_renderer.esSacerdote(*obj)) {
                tiendaSelOferta = tiendaSelInv = -1;
                object_state.abrirTienda(true);
            }
        }
    }

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

    // (Comprar/Vender ahora se hacen en el modal de la tienda, no en el panel.)

    // Boton Equipar: equipa el item seleccionado.
    if (object_renderer.clickEnBotonEquipar(x, y)) {
        if (slotInvSeleccionado >= 0 &&
            slotInvSeleccionado < static_cast<int>(items.size()) &&
            items[slotInvSeleccionado] != 0) {
            despacharComando({Opcode::EQUIPAR,
                              ComandoEquipar{static_cast<uint8_t>(slotInvSeleccionado)}}, tick);
            object_state.mensajeLocal("Equipaste " + catalogo.nombre(items[slotInvSeleccionado]) +
                                      ".", TipoMensajeChat::Normal);
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
        } else {
            object_state.mensajeLocal("Selecciona una pocion del inventario para usar.",
                                      TipoMensajeChat::Sistema);
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
        object_state.mensajeLocal("Lanzaste un hechizo.", TipoMensajeChat::Hechizo);
        // El FX lo difunde el server (FX_HECHIZO) para que lo vean todos, incluido el que lanza.
        return;
    }

    if (object_renderer.slotEquipoClickeado(x, y) >= 0) {
        object_state.mensajeLocal("Los items equipados no se usan desde equipo.",
                                  TipoMensajeChat::Sistema);
        return;
    }

    // Click en el inventario -> seleccionar (zoom).
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

void ClientGameLoop::manejarEventoTienda(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        is_running = false;
        return;
    }
    if (event.type != SDL_MOUSEBUTTONDOWN) {
        return;
    }
    const uint32_t tick = SDL_GetTicks();
    const std::optional<uint16_t> npc = handler.objetivoSeleccionado();
    const int x = event.button.x;
    const int y = event.button.y;

    if (object_renderer.clickTiendaCerrar(x, y)) {
        object_state.cerrarTienda();
        tiendaSelOferta = tiendaSelInv = -1;
        return;
    }
    if (const int o = object_renderer.tiendaOfertaClickeada(x, y); o >= 0) {
        tiendaSelOferta = (tiendaSelOferta == o) ? -1 : o;
        return;
    }
    if (const int iv = object_renderer.tiendaInvClickeado(x, y); iv >= 0) {
        tiendaSelInv = (tiendaSelInv == iv) ? -1 : iv;
        return;
    }

    if (object_state.tiendaEsSacerdote()) {
        if (object_renderer.clickTiendaComprar(x, y) && npc && tiendaSelOferta >= 0) {
            std::vector<uint16_t> ids = catalogo.idsHechizos();
            std::sort(ids.begin(), ids.end());
            if (tiendaSelOferta < static_cast<int>(ids.size())) {
                despacharComando({Opcode::COMPRAR_HECHIZO,
                                  ComandoComprarHechizo{ids[tiendaSelOferta], *npc}}, tick);
            }
        }
        return;
    }


    if (object_renderer.clickTiendaComprar(x, y)) {
        const std::vector<uint16_t>& stock = object_state.stockNpc();
        if (npc && tiendaSelOferta >= 0 && tiendaSelOferta < static_cast<int>(stock.size())) {
            despacharComando({Opcode::COMPRAR, ComandoComprar{stock[tiendaSelOferta], *npc}}, tick);
            tiendaSelOferta = -1;
        }
        return;
    }
    if (object_renderer.clickTiendaVender(x, y)) {
        const std::vector<uint16_t>& inv = object_state.inventario();
        if (npc && tiendaSelInv >= 0 && tiendaSelInv < static_cast<int>(inv.size()) &&
            inv[tiendaSelInv] != 0) {
            despacharComando({Opcode::VENDER,
                              ComandoVender{static_cast<uint8_t>(tiendaSelInv), *npc}}, tick);
            tiendaSelInv = -1;
        }
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
        case Opcode::LISTAR: {
            const std::optional<uint16_t> objetivo = handler.objetivoSeleccionado();
            if (!objetivo.has_value()) {
                break;
            }
            if (object_renderer.esBanquero(*objetivo)) {
                gestorAudio->reproducirEfecto("banqueroBienvenida");
            } else if (object_renderer.esSacerdote(*objetivo)) {
                gestorAudio->reproducirEfecto("sacerdoteBienvenida");
            } else if (object_renderer.esComerciante(*objetivo)) {
                gestorAudio->reproducirEfecto("comercianteBienvenida");
            }
            break;
        }
        case Opcode::CURAR:
            gestorAudio->reproducirEfecto("sacerdoteInteraccion");
            break;
        case Opcode::DEPOSITAR_ITEM:
        case Opcode::DEPOSITAR_ORO:
        case Opcode::RETIRAR_ITEM:
        case Opcode::RETIRAR_ORO:
            gestorAudio->reproducirEfecto("banqueroInteraccion");
            break;
        default:
            break;
    }
}

void ClientGameLoop::update(const int it) {
    const uint32_t current_tick = SDL_GetTicks();
    try {
        object_state.upload_server_msg(server_messages, current_tick, *gestorAudio);
    } catch (const ClosedQueue&) {
        servidorCaido_ = true;
        is_running = false;
        return;
    }
    // El jugador cambia de mapa (pasa por el portal)
    const uint16_t mapaAhora = object_state.mapaActual();
    object_renderer.setMapaActual(mapaAhora);
    handler.setMapaDimensiones(object_renderer.anchoMapa(), object_renderer.altoMapa());

    // Audio por contexto de mapa: musica de mazmorra vs campo, y pasos de caverna vs normales (ambos idempotentes). Al cambiar de mapa, un golpe de transicion de portal.
    const bool enMazmorra = (mapaAhora != object_renderer.getMapaPrincipal());
    gestorAudio->reproducirMusica(enMazmorra ? "mazmorra" : "campo");
    gestorAudio->setClavePasos(enMazmorra ? "entrarCaverna" : "pasos");
    if (mapaAhora != mapaAnteriorAudio) {
        gestorAudio->reproducirEfecto("transicionPortal");
        mapaAnteriorAudio = mapaAhora;
    }
    
    for (const auto& [idHechizo, idObjetivo] : object_state.drenarFx()) {
        object_renderer.iniciarFx(idHechizo, idObjetivo);
    }
    for (const auto& [idOrigen, idDestino] : object_state.drenarProyectiles()) {
        object_renderer.iniciarProyectil(idOrigen, idDestino);
    }
    object_renderer.update_animation(it, object_state, object_animation);
}

void ClientGameLoop::render() {
    // Solo lo dinamico: la config del chat ya la tiene el renderer desde init().
    EstadoChatRender chat;
    chat.activo = handler.chatActivo();
    chat.entrada = handler.bufferChat();
    const std::deque<LineaChat>& historial = object_state.historialChat();
    chat.historial.assign(historial.begin(), historial.end());

    EstadoPanelRender panel;
    panel.nick = object_state.nick();
    const EstadoJugador& st = object_state.estadoJugador();
    if (st.raza < config.razasNombres.size()) {
        panel.raza = config.razasNombres[st.raza];
    }
    if (st.clase < config.clasesNombres.size()) {
        panel.clase = config.clasesNombres[st.clase];
    }
    panel.inventario = object_state.inventario();
    panel.equip = object_state.equipamiento();
    panel.stats = object_state.estadoJugador();
    panel.seleccionInventario = slotInvSeleccionado;
    panel.hechizosConocidos = object_state.hechizosConocidos();
    panel.mostrarHechizos = pestanaHechizos;
    panel.stock.clear();
    panel.sacerdoteSeleccionado = false;
    const std::optional<uint16_t> objetivoPanel = handler.objetivoSeleccionado();


    uint16_t objetivoResaltado = 0;
    if (objetivoPanel.has_value()) {
        const auto& ents = object_state.entities();
        const auto it = ents.find(*objetivoPanel);
        if (it != ents.end()) {
            const int dist = std::abs(static_cast<int>(it->second.x) - object_state.player_x()) +
                             std::abs(static_cast<int>(it->second.y) - object_state.player_y());
            if (dist <= config.seleccionRango) {
                objetivoResaltado = *objetivoPanel;
            }
        }
    }
    object_renderer.resaltarObjetivo(objetivoResaltado);

    uint16_t hoverId = 0;
    if (!object_state.bancoRecibido() && !object_state.tiendaAbierta()) {
        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX < object_renderer.bordeIzquierdoPanel()) {  // no sobre el panel derecho
            hoverId = handler.idEntidadEn(mouseX, mouseY, object_state.entities());
        }
    }
    object_renderer.resaltarHover(hoverId);

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

    EstadoTiendaRender tienda;
    tienda.abierto = object_state.tiendaAbierta();
    if (tienda.abierto) {
        tienda.esSacerdote = object_state.tiendaEsSacerdote();
        if (tienda.esSacerdote) {
            std::vector<uint16_t> ids = catalogo.idsHechizos();
            std::sort(ids.begin(), ids.end());
            tienda.oferta = ids;
            tienda.inventario = object_state.hechizosConocidos();
        } else {
            tienda.oferta = object_state.stockNpc();       // lo que vende el comerciante
            tienda.inventario = object_state.inventario();  // lo del jugador (vender)
        }
        tienda.selOferta = tiendaSelOferta;
        tienda.selInventario = tiendaSelInv;
    }

    object_renderer.render(object_state, object_animation, chat, panel, banco, tienda);
    handler.setCamaraTransform(object_renderer.camOffsetX(), object_renderer.camOffsetY(),
                               object_renderer.camTileW(), object_renderer.camTileH());
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
