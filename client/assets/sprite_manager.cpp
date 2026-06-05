//
// Created by victoria zubieta on 26/05/2026.
//

#include "sprite_manager.h"

#include <algorithm>

SpriteManager::SpriteManager(const int animation_fps, const int loop_fps):
        current_frame(0),
        frames_per_animation_step(
                (animation_fps > 0 && loop_fps > 0) ? std::max(1, loop_fps / animation_fps) : 1),
        animaciones(4) {}

void SpriteManager::update(const uint32_t frame_counter, const int current_row) {
    if (current_row >= static_cast<int>(animaciones.size())) {
        return;
    }

    const size_t frame_count = animaciones[current_row].size();
    if (frame_count == 0) {
        return;
    }

    current_frame = static_cast<int>((frame_counter / frames_per_animation_step) % frame_count);
}

void SpriteManager::add_animation(const int row_idx, const int frame_count, const int frame_width,
                                  const int frame_height, const int x_offset, const int y_offset,
                                  const int x_step, const int y_step) {
    if (row_idx >= static_cast<int>(animaciones.size())) {
        animaciones.resize(row_idx + 1);
    }

    for (int i = 0; i < frame_count; i++) {
        animaciones[row_idx].emplace_back(
                x_offset + (i * x_step),
                y_offset + (i * y_step),
                frame_width,
                frame_height);
    }
}

void SpriteManager::render(SDL2pp::Renderer& renderer,
                           SDL2pp::Texture& texture,
                           const int x, const int y, const int row,
                           const float scale) const {
    if (row >= static_cast<int>(animaciones.size()) || animaciones[row].empty()) {
        return;
    }

    const int frame_to_draw = current_frame % animaciones[row].size();
    const SDL2pp::Rect& src_rect = animaciones[row][frame_to_draw];
    SDL2pp::Rect dst_rect(x, y,
                          static_cast<int>(scale * src_rect.w),
                          static_cast<int>(scale * src_rect.h));

    renderer.Copy(texture, src_rect, dst_rect);
}

int SpriteManager::current_frame_index() const {
    return current_frame;
}

void SpriteManager::reset_frame() {
    current_frame = 0;
}
