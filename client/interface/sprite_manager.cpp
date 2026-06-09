//
// Created by victoria zubieta on 26/05/2026.
//

#include "sprite_manager.h"

#include <algorithm>

SpriteManager::SpriteManager(const int animation_fps, const int loop_fps):
        current_frame(0),
        frames_per_animation_step(
                (animation_fps > 0 && loop_fps > 0) ? std::max(1, loop_fps / animation_fps) : 1) {}

void SpriteManager::update(const uint32_t frame_counter, const int frame_count) {
    if (frame_count <= 0) {
        return;
    }
    current_frame = static_cast<int>((frame_counter / frames_per_animation_step) % frame_count);
}

int SpriteManager::current_frame_index() const {
    return current_frame;
}

void SpriteManager::reset_frame() {
    current_frame = 0;
}
