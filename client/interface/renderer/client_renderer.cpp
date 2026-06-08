//
// Created by victoria zubieta on 29/05/2026.
//

#include "client_renderer.h"

#include <iostream>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Surface.hh"
#include "SDL_image.h"
#include "../../common/persistencia/lector_mapa.h"

#define SPRITE_ANIMATION_FPS 8

#ifndef CLIENT_INTERFACE_DIR
#define CLIENT_INTERFACE_DIR "client/interface"
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

void ObjectRenderer::init(const char* title, const int xpos, const int ypos,
                          const int width, const int height, const bool fullscreen,
                          const bool vsync, const int loop_fps) {

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
    camera.configure(window_width, window_height, mapa.getAncho(), mapa.getAlto());

    try {
        const std::string background_path =
                std::string(CLIENT_INTERFACE_DIR) + "/../resources/mapas/pasto.png";
        SDL2pp::Surface background_surface(background_path);
        background_texture = std::make_unique<SDL2pp::Texture>(*renderer, background_surface);
        const std::string city_path =
                std::string(CLIENT_INTERFACE_DIR) + "/../resources/mapas/6061.png";
        SDL2pp::Surface city_surface(city_path);
        city_texture = std::make_unique<SDL2pp::Texture>(*renderer, city_surface);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el fondo: " << e.what() << std::endl;
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

void ObjectRenderer::update_animation( const int it,
                                      const ObjectGameWorld& state_object,
                                      const ObjectAnimation& animation) {
    if (!sprite_manager) { return;}
    camera.center_on_tile(state_object.player_x(), state_object.player_y());

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

void ObjectRenderer::render(const ObjectGameWorld& state_object, const ObjectAnimation& /*animation*/) {
    if (!renderer) { return; }



    if (background_texture) {
        renderer->Clear();
        // Aclara ligeramente el fondo para mejorar la lectura de criaturas y NPCs.
        SDL_SetTextureColorMod(background_texture->Get(), 155, 155, 155);
        renderer->Copy(*background_texture, SDL2pp::NullOpt,
                       SDL2pp::Rect(0, 0, window_width, window_height));
    } else {
        renderer->SetDrawColor(0, 255, 0, 255);
        renderer->Clear();
    }

    for (const auto& ciudad : mapa.getCiudades()) {
        if (!camera.is_visible(ciudad.xMin, ciudad.yMin)) continue;
        const int cell_width = camera.tile_width();
        const int cell_height = camera.tile_height();
        const int city_x = camera.screen_x_for_tile(ciudad.xMin);
        const int city_y = camera.screen_y_for_tile(ciudad.yMin);
        const int city_width = (ciudad.xMax - ciudad.xMin + 1) * cell_width;
        const int city_height = (ciudad.yMax - ciudad.yMin + 1) * cell_height;

        if (city_texture) {
            renderer->Copy(*city_texture, SDL2pp::NullOpt,
                           SDL2pp::Rect(city_x, city_y, city_width, city_height));
        } else {
            renderer->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            renderer->SetDrawColor(214, 181, 94, 90);
            renderer->FillRect(SDL2pp::Rect(city_x, city_y, city_width, city_height));
            renderer->SetDrawColor(160, 110, 40, 180);
            renderer->DrawRect(SDL2pp::Rect(city_x, city_y, city_width, city_height));
        }
    }

    for (const auto& wall : mapa.getParedes()) {
        if (!camera.is_visible(wall.x, wall.y)) continue;
        const int cell_width = camera.tile_width();
        const int cell_height = camera.tile_height();
        const int wall_x = camera.screen_x_for_tile(wall.x);
        const int wall_y = camera.screen_y_for_tile(wall.y);

        renderer->SetDrawColor(0, 0, 0, 255); // color de paredes es negro
        renderer->FillRect(SDL2pp::Rect(wall_x, wall_y, cell_width, cell_height));
    }

    for (const auto& [id, sacerdote] : mapa.getSacerdotes()) {
        if (!camera.is_visible(sacerdote.getPosicion().x, sacerdote.getPosicion().y)) continue;
        const int cell_width = camera.tile_width();
        const int cell_height = camera.tile_height();
        const int sacerdote_x = camera.screen_x_for_tile(sacerdote.getPosicion().x);
        const int sacerdote_y = camera.screen_y_for_tile(sacerdote.getPosicion().y);

        if (!npc_renderer) { continue; }

        npc_renderer->render(*renderer, sacerdote, sacerdote_x, sacerdote_y, cell_width, cell_height, 0, 0);
    }

    for (const auto& [id, banquero] : mapa.getBanqueros()) {
        if (!camera.is_visible(banquero.getPosicion().x, banquero.getPosicion().y)) continue;
        const int cell_width = camera.tile_width();
        const int cell_height = camera.tile_height();
        const int banquero_x = camera.screen_x_for_tile(banquero.getPosicion().x);
        const int banquero_y = camera.screen_y_for_tile(banquero.getPosicion().y);

        if (!npc_renderer) { continue; }

        npc_renderer->render(*renderer, banquero, banquero_x, banquero_y, cell_width, cell_height, 0, 0);
    }

    for (const auto& [id, comerciante] : mapa.getComerciantes()) {
        if (!camera.is_visible(comerciante.getPosicion().x, comerciante.getPosicion().y)) continue;
        const int cell_width = camera.tile_width();
        const int cell_height = camera.tile_height();
        const int comerciante_x = camera.screen_x_for_tile(comerciante.getPosicion().x);
        const int comerciante_y = camera.screen_y_for_tile(comerciante.getPosicion().y);

        if (!npc_renderer) { continue;}
        npc_renderer->render(*renderer, comerciante, comerciante_x, comerciante_y, cell_width, cell_height, 0, 0);
    }

    for (const auto& [id, entity] : state_object.entities()) {

        const int cell_width = camera.tile_width();
        const int cell_height = camera.tile_height();
        const int entity_x = camera.screen_x_for_tile(entity.x);
        const int entity_y = camera.screen_y_for_tile(entity.y);

        if (entity.tipo == 0) {
            const int animation_row = state_object.entity_animation_row(id);
            const int frame_index =
                    state_object.entity_is_moving(id) ? state_object.entity_walk_frame(id) : 0;

            if (!character_renderer) { continue; }

            character_renderer->render(*renderer, entity, entity_x, entity_y, cell_width, cell_height, animation_row, frame_index);
            continue;
        }

        if (entity.tipo == 1) {
            if (!criatura_renderer) { continue; }
            criatura_renderer->render(*renderer, entity, entity_x, entity_y, cell_width, cell_height, 0, 0);
            continue;
        }

        const SDL_Color color = elegircolor(entity.tipo, entity.estado);
        renderer->SetDrawColor(color.r, color.g, color.b, color.a);
        renderer->FillRect(SDL2pp::Rect(entity_x, entity_y, cell_width, cell_height));
    }

    renderer->Present();
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
