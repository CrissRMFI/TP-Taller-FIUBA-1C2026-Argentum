//
// Created by victoria zubieta on 26/05/2026.
//

#ifndef TALLER_TP_SPRITE_MANAGER_H
#define TALLER_TP_SPRITE_MANAGER_H
#include <SDL_render.h>

#include "SDL2pp/Texture.hh"


class SpriteManager {
private:
    SDL2pp::Texture& texture;  //ref textura cargada
    int current_frame; // donde esta parado actualmente
    uint32_t frame_rate; // velocidad por animacion
    uint32_t last_frame_tick;
    std::vector<std::vector<SDL2pp::Rect>> animaciones;


public:
    SpriteManager(SDL2pp::Texture& texture, int fps);

    void update(uint32_t current_tick, int current_row);
    void add_animation(int row_idx, int frame_count, int frame_width, int frame_height,
                       int x_offset, int y_offset, int x_step, int y_step = 0);
    void render(SDL2pp::Renderer& renderer,
        int x, int y, int row = 0, float scale = 1.0f) const;

    void reset_frame();
};


#endif //TALLER_TP_SPRITE_MANAGER_H
