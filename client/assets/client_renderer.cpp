//
// Created by victoria zubieta on 29/05/2026.
//

#include "client_renderer.h"

#include <iostream>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Surface.hh"
#include "SDL_image.h"

#define SPRITE_FRAME_WIDTH 20
#define SPRITE_FRAME_HEIGHT 40
#define SPRITE_TOP_PADDING 5
#define SPRITE_FRAME_STEP_X 32
#define SPRITE_ROW_GAP 5
#define SPRITE_ANIMATION_FPS 8

#ifndef CLIENT_ASSETS_DIR
#define CLIENT_ASSETS_DIR "client/assets"
#endif


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
    SDL2pp::SDLImage image_init(IMG_INIT_PNG);
    window = std::make_unique<SDL2pp::Window>(title, xpos, ypos, width, height, flags);
    renderer = std::make_unique<SDL2pp::Renderer>(*window, -1, SDL_RENDERER_ACCELERATED);

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
        std::cerr << "Error al cargar la imagen: " << e.what() << std::endl;
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

    renderer->SetDrawColor(30, 30, 30, 255);
    renderer->Clear();

    for (const auto& [id, entity] : state_object.entities()) {
        if (id == state_object.client_id() && sprite_manager) {
            sprite_manager->render(*renderer, state_object.player_x(), state_object.player_y(),
                                   animation.current_animation_row(), 2.0f);
           // continue;
        }

        // otras entidades del juego (de momento son mini rectangulos)
        // renderer->SetDrawColor(100, 50, 50, 255);
        // SDL_Rect rect = {static_cast<int>(entity.x), static_cast<int>(entity.y), 2, 2};
        // renderer->FillRect(rect);
    }

    renderer->Present();
}
