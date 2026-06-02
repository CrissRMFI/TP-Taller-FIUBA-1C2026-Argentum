//
// Created by victoria zubieta on 26/05/2026.
//

#include "sprite_manager.h"

SpriteManager::SpriteManager(int fps):
        current_frame(0),
        frame_rate(fps > 0 ? 1000 / fps : 0),
        last_frame_tick(0),
        animaciones(4) {}

void SpriteManager::update(const uint32_t current_tick, int current_row) {
    if (frame_rate == 0 || current_row >= static_cast<int>(animaciones.size())) {
        return;
    }

    const size_t frame_count = animaciones[current_row].size();
    if (frame_count == 0) {
        return;
    }
    if (current_tick - last_frame_tick >= frame_rate) {
        current_frame = (current_frame + 1) % frame_count;
        last_frame_tick = current_tick;
    }
}

void SpriteManager::add_animation(const int row_idx, const int frame_count, int frame_width, int frame_height,
    const int x_offset, int y_offset,
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
