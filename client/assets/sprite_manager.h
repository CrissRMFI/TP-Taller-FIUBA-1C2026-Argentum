//
// Created by victoria zubieta on 26/05/2026.
//

#ifndef TALLER_TP_SPRITE_MANAGER_H
#define TALLER_TP_SPRITE_MANAGER_H
#include <SDL_render.h>
#include <vector>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/Texture.hh"

class SpriteManager {
private:
    int current_frame;
    int frames_per_animation_step;
    std::vector<std::vector<SDL2pp::Rect>> animaciones;

public:
    SpriteManager(int animation_fps, int loop_fps);

    void update(uint32_t frame_counter, int current_row);
    void add_animation(int row_idx, int frame_count, int frame_width, int frame_height,
                       int x_offset, int y_offset, int x_step, int y_step = 0);
    void render(SDL2pp::Renderer& renderer,
                SDL2pp::Texture& texture,
                int x, int y, int row = 0, float scale = 1.0f) const;

    int current_frame_index() const;
    void reset_frame();
};

#endif  // TALLER_TP_SPRITE_MANAGER_H
