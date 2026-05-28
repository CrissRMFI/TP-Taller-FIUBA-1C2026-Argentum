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

    int frame_width; //ancho de un solo frame
    int frame_height; // altura de un solo frame
    //int total_frames; //cantidad de frames en la animacion (son las direcciones validas)
    std::array<int, 4> frames_per_row;
    int current_frame; // donde esta parado actualmente
    uint32_t frame_rate; // velocidad por animacion
    uint32_t last_frame_tick;


public:
    SpriteManager(SDL2pp::Texture& textura, int fw, int fh,
        std::array<int,4> frames, int fps);

    void update(uint32_t current_tick, int current_row);
    void render(SDL2pp::Renderer& renderer,
        int x, int y, int row = 0, float scale = 1.0f) const;

    void reset_frame();
};


#endif //TALLER_TP_SPRITE_MANAGER_H
