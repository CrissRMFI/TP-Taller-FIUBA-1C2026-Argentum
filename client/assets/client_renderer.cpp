#include "client_renderer.h"

#include <iostream>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Surface.hh"
#include "SDL_image.h"
#include "../../common/mensajes/mensajes_error_cliente.h"
#include "../../common/persistencia/lector_mapa.h"

#define SPRITE_ANIMATION_FPS 8

#ifndef CLIENT_ASSETS_DIR
#define CLIENT_ASSETS_DIR "client/assets"
#endif

#ifndef CLIENT_MAP_PATH
#define CLIENT_MAP_PATH "config/mapa.toml"
#endif

Mapa ObjectRenderer::cargarMapa() const {
    try {
        LectorMapa lector_mapa;
        return lector_mapa.leer(CLIENT_MAP_PATH).mapa;
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el mapa '" << CLIENT_MAP_PATH << "': " << e.what()
                  << ". Se usa un mapa vacio." << std::endl;
        return Mapa(100, 100);
    }
}

ObjectRenderer::ObjectRenderer() : mapa(cargarMapa()) {}

void ObjectRenderer::init(const char* title,
                          const int xpos,
                          const int ypos,
                          const int width,
                          const int height,
                          const bool fullscreen,
                          const bool vsync,
                          const int loop_fps,
                          const ConfigChatRender& chat_config,
                          const ConfigPanelRender& panel_config,
                          const CatalogoItems* catalogo) {
    this->chat_config = chat_config;
    this->panel_config = panel_config;
    this->catalogo = catalogo;
    uint32_t flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    sdl = std::make_unique<SDL2pp::SDL>(SDL_INIT_VIDEO);
    image_context = std::make_unique<SDL2pp::SDLImage>(IMG_INIT_PNG);
    window = std::make_unique<SDL2pp::Window>(title, xpos, ypos, width, height, flags);

    
    uint32_t renderer_flags = SDL_RENDERER_ACCELERATED;
    if (vsync) {
        renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
    }
    renderer = std::make_unique<SDL2pp::Renderer>(*window, -1, renderer_flags);
    window_width = width;
    window_height = height;

    try {
        const std::string background_path =
                std::string(CLIENT_ASSETS_DIR) + "/../resources/mapas/pasto.png";
        SDL2pp::Surface background_surface(background_path);
        background_texture = std::make_unique<SDL2pp::Texture>(*renderer, background_surface);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el fondo: " << e.what() << std::endl;
    }

    try {
        const std::string fuente_path =
                std::string(CLIENT_ASSETS_DIR) + "/../resources/" + chat_config.fuenteRuta;
        text_renderer = std::make_unique<TextRenderer>(fuente_path, chat_config.fuenteTam);
    } catch (const std::exception& e) {
        // Sin fuente el chat no se dibuja, pero el juego sigue andando.
        std::cerr << "[cliente] "
                  << MensajesErrorCliente::mensaje(CodigoErrorCliente::FUENTE_NO_CARGADA) << ": "
                  << e.what() << std::endl;
    }

    try {
        const std::string fondo_path =
                std::string(CLIENT_ASSETS_DIR) + "/../resources/" + chat_config.fondoRuta;
        SDL2pp::Surface fondo_surface(fondo_path);
        chat_background_texture = std::make_unique<SDL2pp::Texture>(*renderer, fondo_surface);
    } catch (const std::exception& e) {
        // Sin panel el chat igual se dibuja, solo que sin fondo.
        std::cerr << "[cliente] "
                  << MensajesErrorCliente::mensaje(CodigoErrorCliente::FONDO_CHAT_NO_CARGADO)
                  << ": " << e.what() << std::endl;
    }

    try {
        const std::string resources_root = std::string(CLIENT_ASSETS_DIR) + "/../resources";
        const std::string sprites_config_path = resources_root + "/config/sprites.toml";
        catalog = std::make_unique<SpriteCatalog>(
                SpriteCatalog::load_from_file(sprites_config_path));
        cache_texture = std::make_unique<TextureCache>(*renderer, resources_root);
        resolver_sprite =
                std::make_unique<CharacterSpriteResolver>(*catalog, *cache_texture);

        character_renderer = std::make_unique<CharacterRenderer>(*resolver_sprite);

        creature_sprite_resolver =
                std::make_unique<CreatureSpriteResolver>(*catalog, *cache_texture);

        criatura_renderer = std::make_unique<CriaturaRenderer>(*creature_sprite_resolver);
        npc_sprite_resolver = std::make_unique<NpcSpriteResolver>(*catalog, *cache_texture);
        npc_renderer = std::make_unique<NPCRenderer>(*npc_sprite_resolver);

        sprite_manager = std::make_unique<SpriteManager>(SPRITE_ANIMATION_FPS, loop_fps);

        const SkinPreset& default_skin = catalog->skin_preset("humano_default");

        if (!default_skin.body_ids.empty()) {
            const CharacterPartDefinition& body_definition =
                    catalog->body(default_skin.body_ids.front());
            const SpriteVec2 frame_size = body_definition.frame_size.value_or(
                    SpriteVec2{body_definition.visible_size.x, body_definition.visible_size.y});
            for (int row = 0; row < 4; ++row) {
                if (!body_definition.rows[row].has_value()) {
                    continue;
                }
                sprite_manager->add_animation(
                        row, body_definition.rows[row]->frames, frame_size.x, frame_size.y, 0,
                        body_definition.rows[row]->y, body_definition.rows[row]->step_x);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el sprite del jugador: " << e.what() << std::endl;
    }

    window->Raise();
}

void ObjectRenderer::update_animation(/*const uint32_t current_tick*/ const int it,
                                      const ObjectGameWorld& state_object,
                                      const ObjectAnimation& animation) {
    if (!sprite_manager) {
        return;
    }

    bool has_moving_character = false;
    int current_row = animation.current_animation_row();
    for (const auto& [id, entity] : state_object.entities()) {
        if (entity.tipo != 0 || !state_object.entity_is_moving(id)) {
            continue;
        }
        current_row = state_object.entity_animation_row(id);
        has_moving_character = true;
        break;
    }

    if (current_row != last_animation_row) {
        sprite_manager->reset_frame();
        last_animation_row = current_row;
    }

    if (has_moving_character) {
        sprite_manager->update(it, current_row);
    } else {
        sprite_manager->reset_frame();
    }
}

void ObjectRenderer::render(const ObjectGameWorld& state_object,
                            const ObjectAnimation& /*animation*/,
                            const EstadoChatRender& chat,
                            const EstadoPanelRender& panel,
                            const EstadoBancoRender& banco) {
    if (!renderer) {
        return;
    }
    const uint32_t current_tick = SDL_GetTicks();
    // El mundo se dibuja en el area de juego: a la izquierda del panel y DEBAJO del chat.
    const int gw = ancho_juego();
    const int gy0 = chat_config.panelAlto;
    const int gh = std::max(1, window_height - gy0);

    renderer->SetDrawColor(0, 0, 0, 255);
    renderer->Clear();
    // El mundo se recorta al area de juego: asi los sprites (mas grandes que la celda) no se desbordan por debajo del panel ni del chat.
    renderer->SetClipRect(SDL2pp::Rect(0, gy0, gw, gh));
    if (background_texture) {
        // Aclara ligeramente el fondo para mejorar la lectura de criaturas y NPCs.
        SDL_SetTextureColorMod(background_texture->Get(), 155, 155, 155);
        renderer->Copy(*background_texture, SDL2pp::NullOpt,
                       SDL2pp::Rect(0, gy0, gw, gh));
    }
    for (const auto& wall : mapa.getParedes()) {
        const int cell_width = std::max(1, gw / mapa.getAncho());
        const int cell_height = std::max(1, gh / mapa.getAlto());
        const int wall_x = wall.x * gw / mapa.getAncho();
        const int wall_y = wall.y * gh / mapa.getAlto() + gy0;

        renderer->SetDrawColor(0, 0, 0, 255); // color de paredes es negro
        renderer->FillRect(SDL2pp::Rect(wall_x, wall_y, cell_width, cell_height));
    }

    for (const auto& [id, sacerdote] : mapa.getSacerdotes()) {
        const int cell_width = gw / mapa.getAncho();
        const int cell_height = gh / mapa.getAlto();
        const int sacerdote_x = sacerdote.getPosicion().x * gw / mapa.getAncho();
        const int sacerdote_y = sacerdote.getPosicion().y * gh / mapa.getAlto() + gy0;

        if (!npc_renderer) {
            continue;
        }

        npc_renderer->render(*renderer, sacerdote, sacerdote_x, sacerdote_y, cell_width,
                             cell_height, 0, 0);
    }

    for (const auto& [id, banquero] : mapa.getBanqueros()) {
        const int cell_width = gw / mapa.getAncho();
        const int cell_height = gh / mapa.getAlto();
        const int banquero_x = banquero.getPosicion().x * gw / mapa.getAncho();
        const int banquero_y = banquero.getPosicion().y * gh / mapa.getAlto() + gy0;

        if (!npc_renderer) {
            continue;
        }

        npc_renderer->render(*renderer, banquero, banquero_x, banquero_y, cell_width,
                             cell_height, 0, 0);
    }

    for (const auto& [id, comerciante] : mapa.getComerciantes()) {
        const int cell_width = gw / mapa.getAncho();
        const int cell_height = gh / mapa.getAlto();
        const int comerciante_x = comerciante.getPosicion().x * gw / mapa.getAncho();
        const int comerciante_y = comerciante.getPosicion().y * gh / mapa.getAlto() + gy0;

        if (!npc_renderer) {
            continue;
        }
        npc_renderer->render(*renderer, comerciante, comerciante_x, comerciante_y, cell_width,
                             cell_height, 0, 0);
    } 

    for (const auto& [id, entity] : state_object.entities()) {
        const int cell_width = gw / mapa.getAncho();
        const int cell_height = gh / mapa.getAlto();
        const InterpolatedPosition interpolated_position =
                state_object.entity_interpolated_position(id, current_tick);
        const int entity_x = static_cast<int>(interpolated_position.x * gw / mapa.getAncho());
        const int entity_y = gy0 + static_cast<int>(interpolated_position.y * gh / mapa.getAlto());

        if (entity.tipo == 0 && sprite_manager) {
            const int animation_row = state_object.entity_animation_row(id);
            const int frame_index =
                    state_object.entity_is_moving(id) ? sprite_manager->current_frame_index() : 0;
            if (!character_renderer) {
                continue;
            }
            character_renderer->render(*renderer, entity, entity_x, entity_y, cell_width,
                                       cell_height, animation_row, frame_index);
            if (entity.estado == 2) {  // Meditando: aura animada encima del personaje
                dibujar_meditacion(entity_x, entity_y, cell_width, cell_height, current_tick);
            }
            continue;
        }

        if (entity.tipo == 1) {
            if (!criatura_renderer) {
                continue;
            }
            criatura_renderer->render(*renderer, entity, entity_x, entity_y, cell_width,
                                      cell_height, 0, 0);
            continue;
        }

        const SDL_Color color = elegircolor(entity.tipo, entity.estado);
        renderer->SetDrawColor(color.r, color.g, color.b, color.a);
        renderer->FillRect(SDL2pp::Rect(entity_x, entity_y, cell_width, cell_height));
    }

    // Fin del mundo: saco el clip para que el panel y el chat se dibujen completos.
    renderer->SetClipRect(SDL2pp::NullOpt);
    dibujar_panel(panel);
    dibujar_chat(chat);
    if (banco.abierto) {
        dibujar_banco(banco);  // modal sobre todo lo demas
    }

    renderer->Present();
}

int ObjectRenderer::ancho_juego() const {
    const int gw = window_width - panel_config.ancho;
    return (gw > 0) ? gw : window_width;
}

SDL2pp::Texture* ObjectRenderer::icono_item(const uint16_t id) {
    if (id == 0 || !cache_texture || iconos_fallidos.count(id)) {
        return nullptr;
    }
    const std::string ruta = panel_config.iconDir + "/" + std::to_string(id) + ".png";
    try {
        return &cache_texture->get_or_load(ruta);
    } catch (const std::exception& e) {
        iconos_fallidos.insert(id);  // no reintentar ni spamear cada frame
        std::cerr << "[cliente] "
                  << MensajesErrorCliente::mensaje(CodigoErrorCliente::ICONO_ITEM_NO_CARGADO)
                  << " (id=" << id << ": " << e.what() << ")" << std::endl;
        return nullptr;
    }
}

void ObjectRenderer::dibujar_panel(const EstadoPanelRender& panel) {
    if (!renderer || !text_renderer || panel_config.ancho <= 0) {
        return;
    }
    // Rects de slots/boton dibujados este frame (para el hit-test del click).
    slots_inventario.clear();
    slots_stock.clear();
    slots_hechizos.clear();
    ids_hechizos_dibujados.clear();
    slots_hechizos_venta.clear();
    ids_hechizos_venta.clear();
    rect_boton_vender = SDL2pp::Rect(0, 0, 0, 0);
    rect_boton_equipar = SDL2pp::Rect(0, 0, 0, 0);
    rect_boton_usar = SDL2pp::Rect(0, 0, 0, 0);
    rect_boton_curar = SDL2pp::Rect(0, 0, 0, 0);
    const int px = window_width - panel_config.ancho;  // borde izq del panel
    const int pw = panel_config.ancho;
    const int margen = 8;
    const int cx = px + margen;
    const int cw = pw - 2 * margen;
    const int lh = text_renderer->alto_linea();
    const SDL_Color& cTxt = panel_config.colorTexto;
    const SDL_Color& cTit = panel_config.colorTitulo;

    // Fondo de cuero del panel (si falla, rect oscuro).
    try {
        renderer->Copy(cache_texture->get_or_load(panel_config.fondoCuero), SDL2pp::NullOpt,
                       SDL2pp::Rect(px, 0, pw, window_height));
    } catch (const std::exception&) {
        renderer->SetDrawColor(35, 25, 18, 255);
        renderer->FillRect(SDL2pp::Rect(px, 0, pw, window_height));
    }

    int y = margen;
    // --- Header: nivel + oro ---
    const EstadoJugador& s = panel.stats;
    text_renderer->dibujar(*renderer, "Nivel " + std::to_string(s.nivel), cx, y, cTit);
    y += lh;
    text_renderer->dibujar(*renderer, "Oro: " + std::to_string(s.oro), cx, y, cTxt);
    y += lh + 6;

    // --- Barras de vida y mana (con textura de relleno, recortada al valor) ---
    const auto barra = [&](const std::string& etiqueta, uint16_t act, uint16_t max,
                           const std::string& ruta, SDL_Color fallback) {
        text_renderer->dibujar(*renderer, etiqueta + " " + std::to_string(act) + "/" +
                                                  std::to_string(max), cx, y, cTxt);
        y += lh;
        const int h = 14;
        renderer->SetDrawColor(20, 20, 20, 255);
        renderer->FillRect(SDL2pp::Rect(cx, y, cw, h));
        const double pct = (max > 0) ? static_cast<double>(act) / max : 0.0;
        const int w = static_cast<int>(cw * pct);
        if (w > 0) {
            try {
                SDL2pp::Texture& t = cache_texture->get_or_load(ruta);
                const int tw = static_cast<int>(t.GetWidth() * pct);
                renderer->Copy(t, SDL2pp::Rect(0, 0, std::max(1, tw), t.GetHeight()),
                               SDL2pp::Rect(cx, y, w, h));
            } catch (const std::exception&) {
                renderer->SetDrawColor(fallback.r, fallback.g, fallback.b, 255);
                renderer->FillRect(SDL2pp::Rect(cx, y, w, h));
            }
        }
        y += h + 6;
    };
    barra("Vida", s.vida, s.vidaMax, panel_config.barraVida, SDL_Color{200, 40, 40, 255});
    barra("Mana", s.mana, s.manaMax, panel_config.barraMana, SDL_Color{50, 90, 210, 255});

    // Dibuja un slot de tamaño sz (fondo + icono + marco), reusado por equipo e inventario.
    const auto dibujar_slot = [&](int sx, int sy, int sz, uint16_t id) {
        renderer->SetDrawColor(0, 0, 0, 180);
        renderer->FillRect(SDL2pp::Rect(sx, sy, sz, sz));
        if (SDL2pp::Texture* ic = icono_item(id)) {
            renderer->Copy(*ic, SDL2pp::NullOpt, SDL2pp::Rect(sx, sy, sz, sz));
        }
        renderer->SetDrawColor(120, 95, 60, 255);  // marco tipo madera
        renderer->DrawRect(SDL2pp::Rect(sx, sy, sz, sz));
    };

    const int cols = 5;

    // --- Equipo: 5 slots (arma, baculo, defensa, casco, escudo), centrados ---
    const int eq_slot = 32;
    const int eq_gap = 6;
    text_renderer->dibujar(*renderer, "Equipo", cx, y, cTit);
    y += lh + 2;
    const uint16_t equipados[5] = {panel.equip.arma, panel.equip.baculo, panel.equip.defensa,
                                   panel.equip.casco, panel.equip.escudo};
    const int eq_x = px + (pw - (5 * eq_slot + 4 * eq_gap)) / 2;
    for (int i = 0; i < 5; ++i) {
        dibujar_slot(eq_x + i * (eq_slot + eq_gap), y, eq_slot, equipados[i]);
    }
    y += eq_slot + 8;

    // --- Marco con pestañas INVENTARIO / HECHIZOS (la pestaña la trae la imagen) ---
    const int marco_top = y;
    const std::string& marcoImg =
            panel.mostrarHechizos ? panel_config.marcoHechizos : panel_config.marcoInventario;
    int interiorX = px + margen;
    int interiorY = marco_top + lh + 2;
    int interiorW = cw;
    int marco_alto = 0;
    try {
        SDL2pp::Texture& marco = cache_texture->get_or_load(marcoImg);
        const int mw = cw;
        const int mh = (marco.GetWidth() > 0) ? cw * marco.GetHeight() / marco.GetWidth()
                                              : marco.GetHeight();
        renderer->Copy(marco, SDL2pp::NullOpt, SDL2pp::Rect(px + margen, marco_top, mw, mh));
        const int insetX = mw * 5 / 100;
        const int insetY = mh * 11 / 100;
        interiorX = px + margen + insetX;
        interiorY = marco_top + insetY;
        interiorW = mw - 2 * insetX;
        marco_alto = mh;
        // Pestañas clickeables (banda superior del marco): izq = inventario, der = hechizos.
        rect_tab_inv = SDL2pp::Rect(px + margen, marco_top, mw / 2, insetY);
        rect_tab_hech = SDL2pp::Rect(px + margen + mw / 2, marco_top, mw - mw / 2, insetY);
    } catch (const std::exception&) {
        text_renderer->dibujar(*renderer, panel.mostrarHechizos ? "Hechizos" : "Inventario", cx, y,
                               cTit);
        rect_tab_inv = SDL2pp::Rect(cx, y, cw / 2, lh + 2);
        rect_tab_hech = SDL2pp::Rect(cx + cw / 2, y, cw - cw / 2, lh + 2);
    }
    const int marco_fin = marco_top + (marco_alto > 0 ? marco_alto : 300);

    if (panel.mostrarHechizos) {
        // Pestaña HECHIZOS: SOLO los hechizos que el jugador conoce (para lanzar).
        const int fila_h = lh + 4;
        int hy = interiorY;
        if (catalogo != nullptr) {
            for (uint16_t id : panel.hechizosConocidos) {
                if (hy + fila_h > marco_fin) {
                    break;
                }
                const HechizoInfo* h = catalogo->hechizo(id);
                if (h == nullptr) {
                    continue;
                }
                try {
                    SDL2pp::Texture& ic = cache_texture->get_or_load(
                            "imgs/hechizos/" + std::to_string(id) + ".png");
                    renderer->Copy(ic, SDL2pp::NullOpt,
                                   SDL2pp::Rect(interiorX, hy, fila_h - 2, fila_h - 2));
                } catch (const std::exception&) {
                }
                text_renderer->dibujar(*renderer, h->nombre + "  M" + std::to_string(h->mana),
                                       interiorX + fila_h, hy + 2, cTit);
                slots_hechizos.push_back(SDL2pp::Rect(interiorX, hy, interiorW, fila_h));
                ids_hechizos_dibujados.push_back(id);
                hy += fila_h;
            }
        }
    } else {
        // Grilla de inventario calzada en el interior del marco.
        const int inv_gap = 4;
        const int inv_slot = (interiorW - (cols - 1) * inv_gap) / cols;
        for (size_t i = 0; i < panel.inventario.size(); ++i) {
            const int col = static_cast<int>(i) % cols;
            const int row = static_cast<int>(i) / cols;
            const int sx = interiorX + col * (inv_slot + inv_gap);
            const int sy = interiorY + row * (inv_slot + inv_gap);
            dibujar_slot(sx, sy, inv_slot, panel.inventario[i]);
            slots_inventario.push_back(SDL2pp::Rect(sx, sy, inv_slot, inv_slot));

            if (static_cast<int>(i) == panel.seleccionInventario) {
                const int z = inv_slot * 3 / 2;
                const int zx = sx + inv_slot / 2 - z / 2;
                const int zy = sy + inv_slot / 2 - z / 2;
                if (SDL2pp::Texture* ic = icono_item(panel.inventario[i])) {
                    renderer->Copy(*ic, SDL2pp::NullOpt, SDL2pp::Rect(zx, zy, z, z));
                }
                renderer->SetDrawColor(255, 230, 90, 255);
                renderer->DrawRect(SDL2pp::Rect(sx - 1, sy - 1, inv_slot + 2, inv_slot + 2));
            }
        }
    }
    y = marco_fin + 8;

    // --- Botones de accion (imagen; fallback a texto). Devuelven su rect para hit-test. ---
    const auto dibujar_boton = [&](const std::string& ruta, const std::string& etiqueta,
                                   SDL_Color fondoFb) -> SDL2pp::Rect {
        SDL2pp::Rect r;
        try {
            SDL2pp::Texture& b = cache_texture->get_or_load(ruta);
            const int bw = b.GetWidth();
            const int bh = b.GetHeight();
            r = SDL2pp::Rect(px + (pw - bw) / 2, y, bw, bh);
            renderer->Copy(b, SDL2pp::NullOpt, r);
            y += bh + 6;
        } catch (const std::exception&) {
            r = SDL2pp::Rect(cx, y, cw, lh + 6);
            renderer->SetDrawColor(fondoFb.r, fondoFb.g, fondoFb.b, 255);
            renderer->FillRect(r);
            text_renderer->dibujar(*renderer, etiqueta, cx + 4, y + 3, cTit);
            y += lh + 10;
        }
        return r;
    };
    rect_boton_vender = dibujar_boton(panel_config.botonVender, "Vender", {60, 40, 25, 255});
    rect_boton_equipar = dibujar_boton(panel_config.botonEquipar, "Equipar", {45, 55, 35, 255});
    rect_boton_usar = dibujar_boton(panel_config.botonUsar, "Usar", {40, 45, 60, 255});
    rect_boton_curar = dibujar_boton(panel_config.botonCurar, "Curar", {55, 35, 50, 255});

    // --- Comercio: lista clickeable de lo que vende el NPC (con scroll en Y) ---
    if (!panel.stock.empty() && catalogo != nullptr) {
        const int total = static_cast<int>(panel.stock.size());
        const int desde = std::max(0, std::min(panel.scrollStock, total - 1));
        text_renderer->dibujar(*renderer, "Comercio (rueda = scroll)", cx, y, cTit);
        y += lh + 2;
        const int fila_h = lh + 4;
        for (int i = desde; i < total; ++i) {
            if (y + fila_h > window_height) {
                break;
            }
            const uint16_t id = panel.stock[i];
            // Fondo de la fila (para que se note clickeable) + texto.
            renderer->SetDrawColor(0, 0, 0, 110);
            renderer->FillRect(SDL2pp::Rect(cx, y, cw, fila_h));
            const std::string linea =
                    catalogo->nombre(id) + "  $" + std::to_string(catalogo->precioCompra(id));
            text_renderer->dibujar(*renderer, linea, cx + 2, y + 2, cTxt);
            slots_stock.push_back(SDL2pp::Rect(cx, y, cw, fila_h));
            y += fila_h;
        }
    }

    // --- Venta de hechizos del sacerdote: solo los que el jugador AUN no conoce ---
    if (panel.sacerdoteSeleccionado && catalogo != nullptr) {
        std::vector<uint16_t> ids = catalogo->idsHechizos();
        std::sort(ids.begin(), ids.end());
        const int fila_h = lh + 4;
        bool titulo = false;
        for (uint16_t id : ids) {
            const bool conocido =
                    std::find(panel.hechizosConocidos.begin(), panel.hechizosConocidos.end(), id) !=
                    panel.hechizosConocidos.end();
            if (conocido) {
                continue;  // ya lo tiene, no se ofrece
            }
            const HechizoInfo* h = catalogo->hechizo(id);
            if (h == nullptr) {
                continue;
            }
            if (!titulo) {
                if (y + lh + 2 > window_height) {
                    break;
                }
                text_renderer->dibujar(*renderer, "Hechizos (comprar)", cx, y, cTit);
                y += lh + 2;
                titulo = true;
            }
            if (y + fila_h > window_height) {
                break;
            }
            renderer->SetDrawColor(0, 0, 0, 110);
            renderer->FillRect(SDL2pp::Rect(cx, y, cw, fila_h));
            text_renderer->dibujar(*renderer, h->nombre + "  $" + std::to_string(h->precio),
                                   cx + 2, y + 2, cTxt);
            slots_hechizos_venta.push_back(SDL2pp::Rect(cx, y, cw, fila_h));
            ids_hechizos_venta.push_back(id);
            y += fila_h;
        }
    }
}

uint16_t ObjectRenderer::hechizoVentaClickeado(int x, int y) const {
    const int i = slot_en(slots_hechizos_venta, x, y);
    return (i >= 0 && i < static_cast<int>(ids_hechizos_venta.size())) ? ids_hechizos_venta[i] : 0;
}

bool ObjectRenderer::esSacerdote(uint16_t id) const {
    return mapa.getSacerdotes().find(id) != mapa.getSacerdotes().end();
}

uint16_t ObjectRenderer::hechizoClickeado(int x, int y) const {
    const int i = slot_en(slots_hechizos, x, y);
    return (i >= 0 && i < static_cast<int>(ids_hechizos_dibujados.size()))
                   ? ids_hechizos_dibujados[i]
                   : 0;
}

bool ObjectRenderer::clickTabInventario(int x, int y) const {
    const SDL2pp::Rect& r = rect_tab_inv;
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}
bool ObjectRenderer::clickTabHechizos(int x, int y) const {
    const SDL2pp::Rect& r = rect_tab_hech;
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

int ObjectRenderer::slot_en(const std::vector<SDL2pp::Rect>& slots, int x, int y) const {
    for (size_t i = 0; i < slots.size(); ++i) {
        const SDL2pp::Rect& r = slots[i];
        if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int ObjectRenderer::slotInventarioClickeado(int x, int y) const {
    return slot_en(slots_inventario, x, y);
}

int ObjectRenderer::slotStockClickeado(int x, int y) const {
    return slot_en(slots_stock, x, y);
}

bool ObjectRenderer::clickEnBotonVender(int x, int y) const {
    const SDL2pp::Rect& r = rect_boton_vender;
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

bool ObjectRenderer::clickEnBotonEquipar(int x, int y) const {
    const SDL2pp::Rect& r = rect_boton_equipar;
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

bool ObjectRenderer::clickEnBotonUsar(int x, int y) const {
    const SDL2pp::Rect& r = rect_boton_usar;
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

bool ObjectRenderer::clickEnBotonCurar(int x, int y) const {
    const SDL2pp::Rect& r = rect_boton_curar;
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

namespace {
bool dentro(const SDL2pp::Rect& r, int x, int y) {
    return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}
}  // namespace

void ObjectRenderer::dibujar_banco(const EstadoBancoRender& b) {
    if (!renderer || !text_renderer) {
        return;
    }
    banco_boveda.clear();
    banco_inv.clear();

    const int mw = 544;
    const int mh = 481;
    const int mx = (window_width - mw) / 2;
    const int my = (window_height - mh) / 2;
    const SDL_Color& cTxt = panel_config.colorTexto;
    const SDL_Color& cTit = panel_config.colorTitulo;
    const int lh = text_renderer->alto_linea();

    // Fondo de la ventana (si falla, rect oscuro).
    try {
        renderer->Copy(cache_texture->get_or_load(panel_config.bancoImg), SDL2pp::NullOpt,
                       SDL2pp::Rect(mx, my, mw, mh));
    } catch (const std::exception&) {
        renderer->SetDrawColor(20, 15, 10, 255);
        renderer->FillRect(SDL2pp::Rect(mx, my, mw, mh));
    }

    // Grilla calibrable por TOML [banco] (depende de los recuadros del asset).
    const int slot = panel_config.bancoSlot;
    const int gap = panel_config.bancoGap;
    const int cols = panel_config.bancoCols;
    const auto slot_banco = [&](int sx, int sy, uint16_t id, bool sel) {
        renderer->SetDrawColor(0, 0, 0, 150);
        renderer->FillRect(SDL2pp::Rect(sx, sy, slot, slot));
        if (SDL2pp::Texture* ic = icono_item(id)) {
            renderer->Copy(*ic, SDL2pp::NullOpt, SDL2pp::Rect(sx, sy, slot, slot));
        }
        if (sel) {
            renderer->SetDrawColor(255, 230, 90, 255);
        } else {
            renderer->SetDrawColor(120, 95, 60, 255);
        }
        renderer->DrawRect(SDL2pp::Rect(sx, sy, slot, slot));
    };
    const auto grilla = [&](int gx, int gy, const std::vector<uint16_t>& items, int sel,
                            std::vector<SDL2pp::Rect>& rects) {
        for (size_t i = 0; i < items.size(); ++i) {
            const int c = static_cast<int>(i) % cols;
            const int r = static_cast<int>(i) / cols;
            const int sx = gx + c * (slot + gap);
            const int sy = gy + r * (slot + gap);
            slot_banco(sx, sy, items[i], static_cast<int>(i) == sel);
            rects.push_back(SDL2pp::Rect(sx, sy, slot, slot));
        }
    };

    // La imagen ya trae las pestañas BOVEDA / INVENTARIO; solo dibujamos las grillas
    // calzadas en sus recuadros (origen calibrable por TOML).
    grilla(mx + panel_config.bancoBovedaX, my + panel_config.bancoBovedaY, b.boveda, b.selBoveda,
           banco_boveda);
    grilla(mx + panel_config.bancoInvX, my + panel_config.bancoInvY, b.inventario, b.selInventario,
           banco_inv);

    text_renderer->dibujar(*renderer, "Oro banco: " + std::to_string(b.oroBanco), mx + 40,
                           my + 350, cTxt);
    text_renderer->dibujar(*renderer, "Oro mano: " + std::to_string(b.oroJugador), mx + 300,
                           my + 350, cTxt);

    const auto boton = [&](const std::string& ruta, const std::string& txt, int bx, int by,
                           SDL_Color fb) -> SDL2pp::Rect {
        SDL2pp::Rect r;
        try {
            SDL2pp::Texture& t = cache_texture->get_or_load(ruta);
            r = SDL2pp::Rect(bx, by, t.GetWidth(), t.GetHeight());
            renderer->Copy(t, SDL2pp::NullOpt, r);
        } catch (const std::exception&) {
            r = SDL2pp::Rect(bx, by, 120, lh + 8);
            renderer->SetDrawColor(fb.r, fb.g, fb.b, 255);
            renderer->FillRect(r);
            text_renderer->dibujar(*renderer, txt, bx + 4, by + 4, cTit);
        }
        return r;
    };

    // Botones de item (retirar de la boveda / depositar del inventario).
    rect_ret = boton(panel_config.botonRetirar, "Retirar", mx + 40, my + 380, {60, 40, 25, 255});
    rect_dep = boton(panel_config.botonDepositar, "Depositar", mx + 300, my + 380, {45, 55, 35, 255});

    // Caja de monto + botones de oro.
    const int yo = my + 425;
    text_renderer->dibujar(*renderer, "Oro:", mx + 40, yo + 4, cTxt);
    rect_caja_monto = SDL2pp::Rect(mx + 90, yo, 90, lh + 8);
    renderer->SetDrawColor(0, 0, 0, 200);
    renderer->FillRect(rect_caja_monto);
    renderer->SetDrawColor(b.montoActivo ? 255 : 120, b.montoActivo ? 230 : 95, 60, 255);
    renderer->DrawRect(rect_caja_monto);
    text_renderer->dibujar(*renderer, b.monto + (b.montoActivo ? "_" : ""), mx + 94, yo + 4, cTxt);
    rect_dep_oro = boton(panel_config.botonDepositarOro, "Dep oro", mx + 300, yo, {45, 55, 35, 255});
    rect_ret_oro = boton(panel_config.botonRetirarOro, "Ret oro", mx + 190, yo, {60, 40, 25, 255});

    // X para cerrar.
    rect_cerrar_banco = SDL2pp::Rect(mx + mw - 34, my + 10, 26, 26);
    renderer->SetDrawColor(120, 30, 30, 255);
    renderer->FillRect(rect_cerrar_banco);
    text_renderer->dibujar(*renderer, "X", mx + mw - 28, my + 12, cTit);
}

void ObjectRenderer::dibujar_meditacion(int entity_x, int entity_y, int cell_width,
                                        int cell_height, uint32_t tick) {
    if (!cache_texture) {
        return;
    }
    SDL2pp::Texture* tex = nullptr;
    try {
        tex = &cache_texture->get_or_load(panel_config.spriteMeditacion);
    } catch (const std::exception&) {
        return;
    }
    const int cols = 5;
    const int frames = 10;
    const int fw = tex->GetWidth() / cols;  // ~102
    const int rowPitch = 128;
    const int contentH = 110;
    const int idx = static_cast<int>((tick / 120) % frames);
    const int c = idx % cols;
    const int r = idx / cols;
    const SDL2pp::Rect src(c * fw, r * rowPitch, fw, contentH);
    
    const int aw = 40;
    const int ah = 52;
    const int cx = entity_x + cell_width / 2;
    const int feet = entity_y + cell_height;
    const SDL2pp::Rect dst(cx - aw / 2, feet - ah, aw, ah);
    SDL_SetTextureBlendMode(tex->Get(), SDL_BLENDMODE_BLEND);
    renderer->Copy(*tex, src, dst);
}

int ObjectRenderer::bancoBovedaClickeada(int x, int y) const {
    return slot_en(banco_boveda, x, y);
}
int ObjectRenderer::bancoInvClickeado(int x, int y) const { return slot_en(banco_inv, x, y); }
bool ObjectRenderer::clickBancoDepositar(int x, int y) const { return dentro(rect_dep, x, y); }
bool ObjectRenderer::clickBancoRetirar(int x, int y) const { return dentro(rect_ret, x, y); }
bool ObjectRenderer::clickBancoDepositarOro(int x, int y) const { return dentro(rect_dep_oro, x, y); }
bool ObjectRenderer::clickBancoRetirarOro(int x, int y) const { return dentro(rect_ret_oro, x, y); }
bool ObjectRenderer::clickBancoCajaMonto(int x, int y) const { return dentro(rect_caja_monto, x, y); }
bool ObjectRenderer::clickBancoCerrar(int x, int y) const { return dentro(rect_cerrar_banco, x, y); }

void ObjectRenderer::dibujar_chat(const EstadoChatRender& chat) {
    if (!text_renderer || !renderer) {
        return;
    }
    const int alto = text_renderer->alto_linea();
    const int margen = 6;

    // El chat ocupa todo el ancho del area de juego (desde la izq hasta el panel).
    const int panel_ancho = ancho_juego();
    const SDL2pp::Rect caja(chat_config.panelX, chat_config.panelY, panel_ancho,
                            chat_config.panelAlto);
    if (chat_background_texture) {
        renderer->Copy(*chat_background_texture, SDL2pp::NullOpt, caja);
    } else {
        renderer->SetDrawColor(0, 0, 0, 255);
        renderer->FillRect(caja);
    }

    const int inner_x = chat_config.panelX + margen;
    const int caja_fondo = chat_config.panelY + chat_config.panelAlto;

    const int y_input = caja_fondo - alto - margen;
    const std::string prompt = "> " + chat.entrada + (chat.activo ? "_" : "");
    text_renderer->dibujar(*renderer, prompt, inner_x, y_input, chat_config.colorInput);

    // Historial
    int y = y_input - alto;
    const int tope = chat_config.panelY + margen;
    for (auto it = chat.historial.rbegin(); it != chat.historial.rend() && y >= tope; ++it) {
        text_renderer->dibujar(*renderer, *it, inner_x, y, chat_config.colorTexto);
        y -= alto;
    }
}
SDL_Color ObjectRenderer::elegircolor(uint8_t tipo, uint8_t estado) const {
    if (tipo == 0) {
        switch (estado) {
            case 0:
                return {0, 0, 255, 255};
            case 1:
                return {128, 128, 128, 255};
            case 2:
                return {255, 255, 0, 255};
            default:
                break;
        }
   }
    if (tipo == 1) {
        return {255, 0, 0, 255};
    }
    if (tipo == 2) {
        return {0, 180, 0, 255};
    }

    return {255, 255, 255, 255};
}
