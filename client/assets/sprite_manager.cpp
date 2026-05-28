//
// Created by victoria zubieta on 26/05/2026.
//

#include "sprite_manager.h"


#include "SDL2pp/Renderer.hh"

SpriteManager::SpriteManager(SDL2pp::Texture &textura, int fw, int fh, std::array<int,4> frames, int fps) :
 texture(textura),
 frame_width(fw), frame_height(fh), frames_per_row(frames), current_frame(0),
 frame_rate(fps > 0 ? 1000/fps : 0),last_frame_tick(0){}

void SpriteManager::update(const uint32_t current_tick, int current_row) {
 if (frame_rate == 0) return;
 if (current_tick - last_frame_tick >= frame_rate) {
  current_frame =  (current_frame + 1) % (frames_per_row[current_row]);
  last_frame_tick = current_tick;
 }
}
void SpriteManager::render(SDL2pp::Renderer& renderer,
        const int x, const int y, const int row, float scale) const {
//int spacing = 1;
 SDL2pp::Rect src_rect
 (current_frame * (frame_width),
  5+ row * (frame_height + 5), frame_width, frame_height);

 SDL2pp::Rect dst_rect(x, y,
static_cast<int>(frame_width * scale), static_cast<int>(frame_height* scale));

 renderer.Copy(texture, src_rect, dst_rect);
}

void SpriteManager::reset_frame() {
 this->current_frame = 0;
}