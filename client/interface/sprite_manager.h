//
// Created by victoria zubieta on 26/05/2026.
//

#ifndef TALLER_TP_SPRITE_MANAGER_H
#define TALLER_TP_SPRITE_MANAGER_H
#include <SDL_render.h>
class SpriteManager {
private:
    int current_frame;
    int frames_per_animation_step;

public:
    SpriteManager(int animation_fps, int loop_fps);

    void update(uint32_t frame_counter, int frame_count);

    int current_frame_index() const;
    void reset_frame();
};

#endif  // TALLER_TP_SPRITE_MANAGER_H
