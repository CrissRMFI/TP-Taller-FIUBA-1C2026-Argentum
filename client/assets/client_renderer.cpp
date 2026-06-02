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

#define SPRITE_FRAME_WIDTH 20
#define SPRITE_FRAME_HEIGHT 40
#define SPRITE_TOP_PADDING 5
#define SPRITE_FRAME_STEP_X 32
#define SPRITE_ROW_GAP 5
#define SPRITE_ANIMATION_FPS 8

#ifndef CLIENT_ASSETS_DIR
#define CLIENT_ASSETS_DIR "client/assets"
#endif

// Ruta del .bin del escenario (formato AOM1). El cliente carga el MISMO mapa que el servidor: es un asset compartido, no viaja por socket.
#ifndef CLIENT_MAP_PATH
#define CLIENT_MAP_PATH "config/mapa.bin"
#endif

Mapa ObjectRenderer::cargarMapa() const {
    try {
        return LectorMapa::leer(CLIENT_MAP_PATH).mapa;
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
                          const bool fullscreen) {
    uint32_t flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    sdl = std::make_unique<SDL2pp::SDL>(SDL_INIT_VIDEO);
    image_context = std::make_unique<SDL2pp::SDLImage>(IMG_INIT_PNG);
    window = std::make_unique<SDL2pp::Window>(title, xpos, ypos, width, height, flags);
    renderer = std::make_unique<SDL2pp::Renderer>(*window, -1, SDL_RENDERER_ACCELERATED);
    window_width = width;
    window_height = height;

    try {
        const std::string background_path =
                std::string(CLIENT_ASSETS_DIR) + "/../resources/pasto.png";
        SDL2pp::Surface background_surface(background_path);
        background_texture = std::make_unique<SDL2pp::Texture>(*renderer, background_surface);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el fondo: " << e.what() << std::endl;
    }

    try {
        const std::string sprite_path = std::string(CLIENT_ASSETS_DIR) + "/imgs/1071.png";
        SDL2pp::Surface surface(sprite_path);
        texture = std::make_unique<SDL2pp::Texture>(*renderer, surface);
        sprite_manager = std::make_unique<SpriteManager>(*texture, SPRITE_ANIMATION_FPS);

        sprite_manager->add_animation(
                0, 6, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT, 0, SPRITE_TOP_PADDING,
                SPRITE_FRAME_STEP_X);
        sprite_manager->add_animation(
                1, 6, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT, 0,
                SPRITE_TOP_PADDING + (SPRITE_FRAME_HEIGHT + SPRITE_ROW_GAP),
                SPRITE_FRAME_STEP_X);
        sprite_manager->add_animation(
                2, 5, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT, 0,
                SPRITE_TOP_PADDING + 2 * (SPRITE_FRAME_HEIGHT + SPRITE_ROW_GAP),
                SPRITE_FRAME_STEP_X);
        sprite_manager->add_animation(
                3, 5, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT, 0,
                SPRITE_TOP_PADDING + 3 * (SPRITE_FRAME_HEIGHT + SPRITE_ROW_GAP),
                SPRITE_FRAME_STEP_X);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el sprite del jugador: " << e.what() << std::endl;
    }

    window->Raise();
}

void ObjectRenderer::update_animation(const uint32_t current_tick,
                                      const ObjectGameWorld& state_object,
                                      const ObjectAnimation& animation) {
    if (!sprite_manager) {
        return;
    }

    const int current_row = animation.current_animation_row();
    if (current_row != last_animation_row) {
        sprite_manager->reset_frame();
        last_animation_row = current_row;
    }

    if (state_object.player_is_moving()) {
        sprite_manager->update(current_tick, current_row);
    } else {
        sprite_manager->reset_frame();
    }
}

void ObjectRenderer::render(const ObjectGameWorld& state_object, const ObjectAnimation& animation) {
    if (!renderer) {
        return;
    }

    if (background_texture) {
        renderer->Clear();
        renderer->Copy(*background_texture, SDL2pp::NullOpt,
                       SDL2pp::Rect(0, 0, window_width, window_height));
    } else {
        renderer->SetDrawColor(0, 255, 0, 255);
        renderer->Clear();
    }
    for (const auto& wall : mapa.getParedes()) {
        const int cell_width = std::max(1, window_width / mapa.getAncho());
        const int cell_height = std::max(1, window_height / mapa.getAlto());
        const int wall_x = wall.x * window_width / mapa.getAncho();
        const int wall_y = wall.y * window_height / mapa.getAlto();

        renderer->SetDrawColor(0, 0, 0, 255); // color de paredes es negro
        renderer->FillRect(SDL2pp::Rect(wall_x, wall_y, cell_width, cell_height));
    }

    for (const auto& [id, sacerdote] : mapa.getSacerdotes()) {
        const int cell_width = window_width / mapa.getAncho();
        const int cell_height = window_height / mapa.getAlto();
        const int sacerdote_x = sacerdote.getPosicion().x * window_width / mapa.getAncho();
        const int sacerdote_y = sacerdote.getPosicion().y * window_height / mapa.getAlto();

        renderer->SetDrawColor(255, 255, 255, 255); // color de sacerdotes es blanco
        renderer->FillRect(SDL2pp::Rect(sacerdote_x, sacerdote_y, cell_width, cell_height));
    }

    for (const auto& [id, banquero] : mapa.getBanqueros()) {
        const int cell_width = window_width / mapa.getAncho();
        const int cell_height = window_height / mapa.getAlto();
        const int banquero_x = banquero.getPosicion().x * window_width / mapa.getAncho();
        const int banquero_y = banquero.getPosicion().y * window_height / mapa.getAlto();

        renderer->SetDrawColor(128, 128, 128, 255); // color de banqueros es gris
        renderer->FillRect(SDL2pp::Rect(banquero_x, banquero_y, cell_width, cell_height));
    }

    for (const auto& [id, comerciante] : mapa.getComerciantes()) {
        const int cell_width = window_width / mapa.getAncho();
        const int cell_height = window_height / mapa.getAlto();
        const int comerciante_x = comerciante.getPosicion().x * window_width / mapa.getAncho();
        const int comerciante_y = comerciante.getPosicion().y * window_height / mapa.getAlto();

        renderer->SetDrawColor(128, 0, 128, 255); // color de comerciantes es violeta
        renderer->FillRect(SDL2pp::Rect(comerciante_x, comerciante_y, cell_width, cell_height));
    } 

    for (const auto& [id, entity] : state_object.entities()) {
        const int cell_width = window_width / mapa.getAncho();
        const int cell_height = window_height / mapa.getAlto();
        const int entity_x = entity.x * window_width / mapa.getAncho();
        const int entity_y = entity.y * window_height / mapa.getAlto();

        if (entity.tipo == 0 && sprite_manager) {
            const int sprite_width = SPRITE_FRAME_WIDTH * 2;
            const int sprite_height = SPRITE_FRAME_HEIGHT * 2;
            const int sprite_x = entity_x - (sprite_width - cell_width) / 2;
            const int sprite_y = entity_y - (sprite_height - cell_height) / 2;
            const int animation_row =
                    (id == state_object.client_id()) ? animation.current_animation_row() : 0;

            if ((entity.estado == 1 || entity.estado == 3) && texture) { // fantasma o reviviendo
                SDL_SetTextureBlendMode(texture->Get(), SDL_BLENDMODE_BLEND);
                SDL_SetTextureAlphaMod(texture->Get(), 128);
            } else if (entity.estado == 2 && texture) { // meditando
                SDL_SetTextureAlphaMod(texture->Get(), 128);
            } else if (texture) { // vivo
                SDL_SetTextureAlphaMod(texture->Get(), 255);
            }

            sprite_manager->render(*renderer, sprite_x, sprite_y,
                                   animation_row, 0.75f);

            if (texture) {
                SDL_SetTextureAlphaMod(texture->Get(), 255);
            }
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
    } else if (tipo == 1) {
        return {255, 0, 0, 255};
    } else if (tipo == 2) {
        return {0, 180, 0, 255};
    }

    return {255, 255, 255, 255};
}
