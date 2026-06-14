#include "client_renderer.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Surface.hh"
#include "SDL_image.h"
#include "../../../common/persistencia/lector_mapa.h"

#define SPRITE_ANIMATION_FPS 8

#ifndef CLIENT_INTERFACE_DIR
#define CLIENT_INTERFACE_DIR "client/interface"
#endif

#ifndef CLIENT_MAP_PATH
#define CLIENT_MAP_PATH "config/mapa.toml"
#endif

std::map<uint16_t, Mapa> ObjectRenderer::cargarMapas() const {
    try {
        LectorMapa lector_mapa;
        // El cliente carga el escenario completo (exterior + mazmorras) para poder
        // cambiar la capa de tiles cuando el jugador atraviesa un portal.
        return lector_mapa.leerMundo(CLIENT_MAP_PATH).mapas;
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el mapa '" << CLIENT_MAP_PATH << "': " << e.what()
                  << ". Se usa un mapa vacio." << std::endl;
        std::map<uint16_t, Mapa> fallback;
        fallback.emplace(0, Mapa(100, 100));
        return fallback;
    }
}

const Mapa& ObjectRenderer::mapaVigente() const {
    const auto it = mapas.find(mapaActual);
    if (it != mapas.end()) {
        return it->second;
    }
    // Si el mapa pedido no existe (no deberia pasar), caemos al primero disponible.
    return mapas.begin()->second;
}

ObjectRenderer::ObjectRenderer() : mapas(cargarMapas()) {}

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
                          const CatalogoItems* catalogo,
                          const ConfigCamara& camara_config,
                          const uint32_t walk_tile_ms) {
    this->chat_config = chat_config;
    this->panel_config = panel_config;
    this->catalogo = catalogo;
    this->walk_tile_ms = (walk_tile_ms > 0) ? walk_tile_ms : 130;
    camera.aplicar_config(camara_config);
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
                std::string(CLIENT_INTERFACE_DIR) + "/../resources/mapas/pasto.png";
        SDL2pp::Surface background_surface(background_path);
        background_texture = std::make_unique<SDL2pp::Texture>(*renderer, background_surface);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el fondo: " << e.what() << std::endl;
    }

    try {
        const std::string fuente_path =
                std::string(CLIENT_INTERFACE_DIR) + "/../resources/" + chat_config.fuenteRuta;
        text_renderer = std::make_unique<TextRenderer>(fuente_path, chat_config.fuenteTam);
    } catch (const std::exception& e) {
        // Sin fuente el chat no se dibuja, pero el juego sigue andando.
        std::cerr << "[cliente] no se pudo cargar la fuente del chat: " << e.what()
                  << std::endl;
    }

    try {
        const std::string fondo_path =
                std::string(CLIENT_INTERFACE_DIR) + "/../resources/" + chat_config.fondoRuta;
        SDL2pp::Surface fondo_surface(fondo_path);
        chat_background_texture = std::make_unique<SDL2pp::Texture>(*renderer, fondo_surface);
    } catch (const std::exception& e) {
        // Sin panel el chat igual se dibuja, solo que sin fondo.
        std::cerr << "[cliente] no se pudo cargar el fondo del chat: " << e.what()
                  << std::endl;
    }

    try {
        const std::string resources_root = std::string(CLIENT_INTERFACE_DIR) + "/../resources";
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
    int current_frame_count = 0;
    for (const auto& [id, entity] : state_object.entities()) {
        if (entity.tipo != 0 || !state_object.entity_is_moving(id)) {
            continue;
        }
        current_row = state_object.entity_animation_row(id);
        if (catalog && catalog->has_body(entity.cuerpo)) {
            const CharacterPartDefinition& body_definition = catalog->body(entity.cuerpo);
            if (body_definition.rows[current_row].has_value()) {
                current_frame_count = body_definition.rows[current_row]->frames;
            }
        }
        has_moving_character = true;
        break;
    }

    if (current_row != last_animation_row) {
        sprite_manager->reset_frame();
        last_animation_row = current_row;
    }

    if (has_moving_character && current_frame_count > 0) {
        sprite_manager->update(it, current_frame_count);
    } else {
        sprite_manager->reset_frame();
    }
}

void ObjectRenderer::actualizar_pos_visual(const int tile_x, const int tile_y,
                                           const uint32_t now_tick) {
    const double speed = 1000.0 / static_cast<double>(walk_tile_ms);  // tiles por segundo

    if (!vis_init) {
        vis_player_x = tile_x;
        vis_player_y = tile_y;
        vis_init = true;
        vis_last_tick = now_tick;
        return;
    }

    double dt = (now_tick - vis_last_tick) / 1000.0;
    vis_last_tick = now_tick;
    dt = std::clamp(dt, 0.0, 0.1);  // si el loop se frena, no pegamos un salto

    const double rem_x = tile_x - vis_player_x;
    const double rem_y = tile_y - vis_player_y;
    const double restante = std::abs(rem_x) + std::abs(rem_y);

    constexpr double EPS = 1e-4;
    if (restante < EPS) {
        return;  // quieto y alineado
    }
    if (restante > 2.5) {  // teleport (muerte/resurreccion/login): ir de una
        vis_player_x = tile_x;
        vis_player_y = tile_y;
        return;
    }

    double paso = speed * dt;
    if (restante > 1.5) {
        paso *= 2.0;  // quedo atras (giro/lag): alcanzar sin arrastrar
    }

    
    const bool x_en_curso = std::abs(vis_player_x - std::round(vis_player_x)) > EPS;
    const auto avanzar = [&](double& v, double objetivo) {
        const double d = objetivo - v;
        v = (std::abs(d) <= paso) ? objetivo : v + (d > 0 ? paso : -paso);
    };
    if (std::abs(rem_x) > EPS && (x_en_curso || std::abs(rem_y) <= EPS)) {
        avanzar(vis_player_x, tile_x);
    } else {
        avanzar(vis_player_y, tile_y);
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
    
    const Mapa& mapa = mapaVigente();
    const uint32_t current_tick = SDL_GetTicks();
    
    const int gw = ancho_juego();
    const int gy0 = chat_config.panelAlto;
    const int gh = std::max(1, window_height - gy0);

    camera.configure(gw, gh, mapa.getAncho(), mapa.getAlto());
    actualizar_pos_visual(state_object.player_x(), state_object.player_y(), current_tick);
    camera.center_on_point(vis_player_x, vis_player_y);
    const int tileW = camera.tile_width();
    const int tileH = camera.tile_height();
    const int camX = camera.get_offset_x();
    const int camY = gy0 + camera.get_offset_y();
    const auto scrX = [&](double tile) { return camX + static_cast<int>(tile * tileW); };
    const auto scrY = [&](double tile) { return camY + static_cast<int>(tile * tileH); };

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
    // --- Terreno por zona (sobre el pasto base): desierto y piso de ciudad, tileados ---
    const auto rectZona = [&](const Ciudad& z) {
        const int sx = scrX(z.xMin);
        const int sy = scrY(z.yMin);
        const int ex = scrX(z.xMax + 1);
        const int ey = scrY(z.yMax + 1);
        return SDL2pp::Rect(sx, sy, std::max(1, ex - sx), std::max(1, ey - sy));
    };
    const auto tileZona = [&](const std::vector<Ciudad>& zonas, const std::string& tex) {
        SDL2pp::Texture* t = nullptr;
        try {
            t = &cache_texture->get_or_load(tex);
        } catch (const std::exception&) {
            return;
        }
        const int paso = 48;
        for (const Ciudad& z : zonas) {
            const SDL2pp::Rect r = rectZona(z);
            renderer->SetClipRect(r);
            for (int yy = r.y; yy < r.y + r.h; yy += paso) {
                for (int xx = r.x; xx < r.x + r.w; xx += paso) {
                    renderer->Copy(*t, SDL2pp::NullOpt, SDL2pp::Rect(xx, yy, paso, paso));
                }
            }
        }
        renderer->SetClipRect(SDL2pp::Rect(0, gy0, gw, gh));  // restaurar clip del mundo
    };
    // Pisos por zona (modelo del editor): cada ZonaPiso tilea su textura sobre el
    // pasto base; el orden del vector resuelve "ultima zona gana".
    const auto texturaPiso = [](const std::string& clave) -> std::string {
        if (clave == "desierto") return "imgs/mapas/desierto.png";
        if (clave == "ciudad")   return "imgs/mapas/ciudad.png";
        return "imgs/mapas/pasto.png";
    };
    {
        const int paso = 48;
        for (const ZonaPiso& z : mapa.getPisos()) {
            if (z.clave == "pasto") continue;  // ya es el fondo base
            SDL2pp::Texture* t = nullptr;
            try {
                t = &cache_texture->get_or_load(texturaPiso(z.clave));
            } catch (const std::exception&) {
                continue;
            }
            const int sx = scrX(z.xMin);
            const int sy = scrY(z.yMin);
            const SDL2pp::Rect r(sx, sy, std::max(1, scrX(z.xMax + 1) - sx),
                                 std::max(1, scrY(z.yMax + 1) - sy));
            renderer->SetClipRect(r);
            for (int yy = r.y; yy < r.y + r.h; yy += paso) {
                for (int xx = r.x; xx < r.x + r.w; xx += paso) {
                    renderer->Copy(*t, SDL2pp::NullOpt, SDL2pp::Rect(xx, yy, paso, paso));
                }
            }
        }
        renderer->SetClipRect(SDL2pp::Rect(0, gy0, gw, gh));  // restaurar clip del mundo
    }
    tileZona(mapa.getCiudades(), "imgs/mapas/ciudad.png");

    
    const auto altoObjeto = [](const std::string& clave) -> double {
        if (clave == "cartel")  return 1.2;
        if (clave == "arbusto") return 1.4;
        return 2.2;  // arboles altos
    };
    for (const ObjetoMapa& o : mapa.getObjetos()) {
        if (!camera.is_visible(o.x, o.y)) continue;
        SDL2pp::Texture* t = nullptr;
        try {
            t = &cache_texture->get_or_load("imgs/mapas/" + o.clave + ".png");
        } catch (const std::exception&) {
            continue;
        }
        const int th = std::max(1, static_cast<int>(tileH * altoObjeto(o.clave)));
        const int tw = std::max(1, th * t->GetWidth() / std::max(1, t->GetHeight()));
        const int cx = scrX(o.x) + tileW / 2;   // centro horizontal de la celda
        const int by = scrY(o.y) + tileH;       // base = borde inferior de la celda
        renderer->Copy(*t, SDL2pp::NullOpt, SDL2pp::Rect(cx - tw / 2, by - th, tw, th));
    }

    // --- Paredes: ladrillo (si falla la textura, rect oscuro) ---
    SDL2pp::Texture* texPared = nullptr;
    try {
        texPared = &cache_texture->get_or_load("imgs/mapas/pared.png");
    } catch (const std::exception&) {
    }
    for (const auto& wall : mapa.getParedes()) {
        if (!camera.is_visible(wall.x, wall.y)) continue;
        const int cell_width = tileW;
        const int cell_height = tileH;
        const int wall_x = scrX(wall.x);
        const int wall_y = scrY(wall.y);
        const SDL2pp::Rect destPared(wall_x, wall_y, cell_width, cell_height);
        if (texPared != nullptr) {
            renderer->Copy(*texPared, SDL2pp::NullOpt, destPared);
        } else {
            renderer->SetDrawColor(0, 0, 0, 255);
            renderer->FillRect(destPared);
        }
    }

    // --- Drops en el piso: oro -> monedas, item -> bolsa/cofre ---
    {
        const int cw_cell = tileW;
        const int ch_cell = tileH;
        const auto dibujarDrop = [&](const std::set<std::pair<uint16_t, uint16_t>>& celdas,
                                     const std::string& tex) {
            SDL2pp::Texture* t = nullptr;
            try {
                t = &cache_texture->get_or_load(tex);
            } catch (const std::exception&) {
                return;
            }
            for (const auto& [cxd, cyd] : celdas) {
                if (!camera.is_visible(cxd, cyd)) continue;
                const int dx = scrX(cxd);
                const int dy = scrY(cyd);
                renderer->Copy(*t, SDL2pp::NullOpt, SDL2pp::Rect(dx, dy, cw_cell, ch_cell));
            }
        };
        dibujarDrop(state_object.oroEnSuelo(), "imgs/suelo/monedas.png");
        dibujarDrop(state_object.itemEnSuelo(), "imgs/suelo/bolsa.png");
    }

    for (const auto& [id, sacerdote] : mapa.getSacerdotes()) {
        const int cell_width = tileW;
        const int cell_height = tileH;
        const int sacerdote_x = scrX(sacerdote.getPosicion().x);
        const int sacerdote_y = scrY(sacerdote.getPosicion().y);

        if (!npc_renderer) {
            continue;
        }

        npc_renderer->render(*renderer, sacerdote, sacerdote_x, sacerdote_y, cell_width,
                             cell_height, 0, 0);
    }

    for (const auto& [id, banquero] : mapa.getBanqueros()) {
        const int cell_width = tileW;
        const int cell_height = tileH;
        const int banquero_x = scrX(banquero.getPosicion().x);
        const int banquero_y = scrY(banquero.getPosicion().y);

        if (!npc_renderer) {
            continue;
        }

        npc_renderer->render(*renderer, banquero, banquero_x, banquero_y, cell_width,
                             cell_height, 0, 0);
    }

    for (const auto& [id, comerciante] : mapa.getComerciantes()) {
        const int cell_width = tileW;
        const int cell_height = tileH;
        const int comerciante_x = scrX(comerciante.getPosicion().x);
        const int comerciante_y = scrY(comerciante.getPosicion().y);

        if (!npc_renderer) {
            continue;
        }
        npc_renderer->render(*renderer, comerciante, comerciante_x, comerciante_y, cell_width,
                             cell_height, 0, 0);
    } 

    for (const auto& [id, entity] : state_object.entities()) {
        const int cell_width = tileW;
        const int cell_height = tileH;
        // El jugador local se dibuja en su posicion visual continua (suave); el resto
        // de las entidades, en su tile (acompañan el scroll suave del mundo).
        const bool es_jugador_local = (id == state_object.client_id());
        const int entity_x = es_jugador_local ? scrX(vis_player_x) : scrX(entity.x);
        const int entity_y = es_jugador_local ? scrY(vis_player_y) : scrY(entity.y);

        const bool resaltar = (objetivo_resaltado != 0 && id == objetivo_resaltado);

        if (entity.tipo == 0 && sprite_manager) {
            const int animation_row = state_object.entity_animation_row(id);
            const int frame_index =
                    state_object.entity_is_moving(id) ? state_object.entity_walk_frame(id) : 0;
            if (!character_renderer) {
                continue;
            }
            character_renderer->render(*renderer, entity, entity_x, entity_y, cell_width,
                                       cell_height, animation_row, frame_index, resaltar);
            if (entity.estado == 2) {  // Meditando: aura animada encima del personaje
                dibujar_meditacion(entity_x, entity_y, cell_width, cell_height, current_tick);
            }
            if (entity.estado == 3) {
                dibujar_resurreccion(entity_x, entity_y, cell_width, cell_height, current_tick);
                if (id == state_object.client_id() && state_object.resurreccionActiva()) {
                    dibujar_barra_resurreccion(
                            entity_x, entity_y, cell_width, cell_height,
                            state_object.fraccionResurreccionRestante(current_tick));
                }
            }
            continue;
        }

        if (entity.tipo == 1) {
            if (!criatura_renderer) {
                continue;
            }
            criatura_renderer->render(*renderer, entity, entity_x, entity_y, cell_width,
                                      cell_height, 0, 0, resaltar);
            continue;
        }
        if (entity.tipo == 2) {
            continue;
        }

        const SDL_Color color = elegircolor(entity.tipo, entity.estado);
        renderer->SetDrawColor(color.r, color.g, color.b, color.a);
        renderer->FillRect(SDL2pp::Rect(entity_x, entity_y, cell_width, cell_height));
    }

    // --- FX de hechizos en curso: animacion transitoria centrada en el objetivo ---
    constexpr uint16_t FX_AURA_BASE = 900;    // ids 900..: auras de critico (imgs/estados/critico)
    constexpr uint16_t FX_ATAQUE_BASE = 7000;  // ids 7000..: swing de ataque (imgs/ataques/fx)
    for (auto it = fx_activos.begin(); it != fx_activos.end();) {
        std::string path;
        int frames = 0;
        int dw = 48;
        int dh = 48;
        if (it->spellId >= FX_ATAQUE_BASE) {
            path = "imgs/ataques/fx/" + std::to_string(it->spellId - FX_ATAQUE_BASE) + ".png";
            frames = 6;  // las tiras de swing se extrajeron a 6 cuadros
            dw = 36;
            dh = 48;
        } else if (it->spellId >= FX_AURA_BASE) {
            path = "imgs/estados/critico/" + std::to_string(it->spellId - FX_AURA_BASE) + ".png";
            frames = 10;  // todas las auras se extrajeron a 10 cuadros
            dw = 72;
            dh = 96;
        } else {
            const HechizoInfo* h = (catalogo != nullptr) ? catalogo->hechizo(it->spellId) : nullptr;
            path = "imgs/hechizos/fx/" + std::to_string(it->spellId) + ".png";
            frames = (h != nullptr) ? static_cast<int>(h->fxFrames) : 0;
        }
        const int frame = (frames > 0) ? static_cast<int>((current_tick - it->startTick) / 60) : 0;
        if (frames <= 0 || frame >= frames ||
            state_object.entities().find(it->targetId) == state_object.entities().end()) {
            it = fx_activos.erase(it);
            continue;
        }
        const auto target = state_object.entities().find(it->targetId);
        if (target == state_object.entities().end()) {
            it = fx_activos.erase(it);
            continue;
        }
        const int tx = scrX(target->second.x);
        const int ty = scrY(target->second.y);
        try {
            SDL2pp::Texture& sheet = cache_texture->get_or_load(path);
            const int cellW = sheet.GetWidth() / frames;
            const int cellH = sheet.GetHeight();
            renderer->Copy(sheet, SDL2pp::Rect(frame * cellW, 0, cellW, cellH),
                           SDL2pp::Rect(tx + tileW / 2 - dw / 2, ty + tileH / 2 - dh, dw, dh));
        } catch (const std::exception&) {
        }
        ++it;
    }

    // --- Proyectiles en vuelo: viajan del origen al destino (~280ms) ---
    constexpr uint32_t PROY_DUR = 280;
    for (auto it = proyectiles_.begin(); it != proyectiles_.end();) {
        const auto& ents = state_object.entities();
        const auto orig = ents.find(it->origen);
        const auto dest = ents.find(it->destino);
        const uint32_t elapsed = current_tick - it->startTick;
        if (elapsed >= PROY_DUR || orig == ents.end() || dest == ents.end()) {
            it = proyectiles_.erase(it);
            continue;
        }
        const float t = static_cast<float>(elapsed) / PROY_DUR;
        const float ox = scrX(orig->second.x + 0.5);
        const float oy = scrY(orig->second.y + 0.5);
        const float dx = scrX(dest->second.x + 0.5);
        const float dy = scrY(dest->second.y + 0.5);
        const int cx = static_cast<int>(ox + (dx - ox) * t);
        const int cy = static_cast<int>(oy + (dy - oy) * t);
        try {
            SDL2pp::Texture& roca = cache_texture->get_or_load("imgs/ataques/proyectil.png");
            const int s = 22;
            renderer->Copy(roca, SDL2pp::NullOpt, SDL2pp::Rect(cx - s / 2, cy - s / 2, s, s));
        } catch (const std::exception&) {
        }
        ++it;
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
        std::cerr << "[cliente] no se pudo cargar el icono del item (id=" << id << "): "
                  << e.what() << std::endl;
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
    // --- Header: nombre (grande), raza/clase (chico), nivel, oro ---
    const EstadoJugador& s = panel.stats;
    if (!panel.nick.empty()) {
        text_renderer->dibujarEscalado(*renderer, panel.nick, cx, y, cTit, 1.8f);
        y += static_cast<int>(lh * 1.8) + 2;
    }
    if (!panel.raza.empty() || !panel.clase.empty()) {
        text_renderer->dibujar(*renderer, panel.raza + " - " + panel.clase, cx, y, cTxt);
        y += lh + 2;
    }
    text_renderer->dibujar(*renderer, "Nivel " + std::to_string(s.nivel), cx, y, cTit);
    text_renderer->dibujar(*renderer, "Oro: " + std::to_string(s.oro), cx + cw / 2, y, cTxt);
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

    // --- Barra de experiencia (exp acumulada / limite del nivel actual) ---
    {
        text_renderer->dibujar(*renderer, "Exp " + std::to_string(s.experiencia) + "/" +
                                                  std::to_string(s.expSiguienteNivel),
                               cx, y, cTxt);
        y += lh;
        const int h = 14;
        renderer->SetDrawColor(20, 20, 20, 255);
        renderer->FillRect(SDL2pp::Rect(cx, y, cw, h));
        const double pct = (s.expSiguienteNivel > 0)
                                   ? std::min(1.0, static_cast<double>(s.experiencia) /
                                                           s.expSiguienteNivel)
                                   : 0.0;
        const int w = static_cast<int>(cw * pct);
        if (w > 0) {
            try {
                SDL2pp::Texture& t = cache_texture->get_or_load(panel_config.barraExperiencia);
                const int tw = static_cast<int>(t.GetWidth() * pct);
                renderer->Copy(t, SDL2pp::Rect(0, 0, std::max(1, tw), t.GetHeight()),
                               SDL2pp::Rect(cx, y, w, h));
            } catch (const std::exception&) {
                renderer->SetDrawColor(200, 170, 40, 255);
                renderer->FillRect(SDL2pp::Rect(cx, y, w, h));
            }
        }
        y += h + 6;
    }

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

    // --- Botones de accion en grilla 2x2 (imagen escalada a la celda; fallback a texto). ---
    const int btn_gap = 6;
    const int btn_col_w = (cw - btn_gap) / 2;
    const int btn_h = 30;
    const int btn_colL = cx;
    const int btn_colR = cx + btn_col_w + btn_gap;
    const auto boton_celda = [&](const std::string& ruta, const std::string& etiqueta,
                                 SDL_Color fondoFb, int bx, int by) -> SDL2pp::Rect {
        SDL2pp::Rect r(bx, by, btn_col_w, btn_h);
        try {
            renderer->Copy(cache_texture->get_or_load(ruta), SDL2pp::NullOpt, r);
        } catch (const std::exception&) {
            renderer->SetDrawColor(fondoFb.r, fondoFb.g, fondoFb.b, 255);
            renderer->FillRect(r);
            text_renderer->dibujar(*renderer, etiqueta, bx + 4, by + (btn_h - lh) / 2, cTit);
        }
        return r;
    };
    // Fila de arriba: Vender | Equipar
    rect_boton_vender = boton_celda(panel_config.botonVender, "Vender", {60, 40, 25, 255},
                                    btn_colL, y);
    rect_boton_equipar = boton_celda(panel_config.botonEquipar, "Equipar", {45, 55, 35, 255},
                                     btn_colR, y);
    y += btn_h + btn_gap;
    // Fila de abajo: Usar | Curar
    rect_boton_usar = boton_celda(panel_config.botonUsar, "Usar", {40, 45, 60, 255}, btn_colL, y);
    rect_boton_curar = boton_celda(panel_config.botonCurar, "Curar", {55, 35, 50, 255},
                                   btn_colR, y);
    y += btn_h + 8;

    // --- Comercio: lista clickeable de lo que vende el NPC (con scroll en Y) ---
    if (!panel.stock.empty() && catalogo != nullptr) {
        const int total = static_cast<int>(panel.stock.size());
        // Permitimos desde==total (0 items) para revelar los hechizos en venta de abajo.
        const int desde = std::max(0, std::min(panel.scrollStock, total));
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
    const Mapa& mapa = mapaVigente();
    return mapa.getSacerdotes().find(id) != mapa.getSacerdotes().end();
}

bool ObjectRenderer::esBanquero(uint16_t id) const {
    const Mapa& mapa = mapaVigente();
    return mapa.getBanqueros().find(id) != mapa.getBanqueros().end();
}

bool ObjectRenderer::esComerciante(uint16_t id) const {
    const Mapa& mapa = mapaVigente();
    return mapa.getComerciantes().find(id) != mapa.getComerciantes().end();
}

void ObjectRenderer::iniciarFx(uint16_t spellId, uint16_t targetId) {
    fx_activos.push_back({spellId, targetId, SDL_GetTicks()});
}

void ObjectRenderer::iniciarProyectil(uint16_t origen, uint16_t destino) {
    proyectiles_.push_back({origen, destino, SDL_GetTicks()});
}

void ObjectRenderer::resaltarObjetivo(uint16_t id) {
    objetivo_resaltado = id;
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

void ObjectRenderer::dibujar_resurreccion(int entity_x, int entity_y, int cell_width,
                                          int cell_height, uint32_t tick) {
    if (!cache_texture) {
        return;
    }
    SDL2pp::Texture* tex = nullptr;
    try {
        tex = &cache_texture->get_or_load(panel_config.spriteResurreccion);
    } catch (const std::exception&) {
        return; 
    }
    
    const int cols = 5;
    const int frames = 15;
    const int cell = tex->GetWidth() / cols;
    const int contentH = cell * 3 / 4;
    const int idx = static_cast<int>((tick / 100) % frames);
    const int c = idx % cols;
    const int r = idx / cols;
    const SDL2pp::Rect src(c * cell, r * cell, cell, contentH);

    const int alturaPersonajePx = 52;
    const int cx = entity_x + cell_width / 2;
    const int feet = entity_y + cell_height;
    const int aw = cell_width * 2;
    const int ah = alturaPersonajePx + 34;
    const SDL2pp::Rect dst(cx - aw / 2, feet - ah, aw, ah);
    SDL_SetTextureBlendMode(tex->Get(), SDL_BLENDMODE_BLEND);
    renderer->Copy(*tex, src, dst);
}

void ObjectRenderer::dibujar_barra_resurreccion(int entity_x, int entity_y, int cell_width,
                                                int cell_height, float fraccion) {
    if (!renderer) {
        return;
    }
    const float f = std::clamp(fraccion, 0.0f, 1.0f);
    const int bw = cell_width;        // ancho de la celda
    const int bh = 5;                 // alto de la barra
    const int bx = entity_x;
    const int alturaPersonajePx = 52;
    const int feet = entity_y + cell_height;
    const int by = feet - alturaPersonajePx - bh - 3;
    // Marco oscuro.
    renderer->SetDrawColor(20, 20, 20, 220);
    renderer->FillRect(SDL2pp::Rect(bx - 1, by - 1, bw + 2, bh + 2));
    const int w = static_cast<int>(bw * f);
    if (w > 0) {
        renderer->SetDrawColor(90, 200, 255, 255);
        renderer->FillRect(SDL2pp::Rect(bx, by, w, bh));
    }
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
        text_renderer->dibujar(*renderer, it->first, inner_x, y, chat_config.colorPara(it->second));
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
