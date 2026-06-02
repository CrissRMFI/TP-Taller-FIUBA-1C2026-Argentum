//
// Created by victoria zubieta on 02/06/2026.
//

#include "criatura_renderer.h"

namespace {
constexpr float CHARACTER_SCALE = 0.25f;
SDL2pp::Rect to_sdl_rect(const SpriteRect& rect) {
    return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
}

}  // namespace

CriaturaRenderer::CriaturaRenderer(CreatureSpriteResolver& resolver):
        resolver_(resolver) {}

void CriaturaRenderer::render(SDL2pp::Renderer& renderer, const EntidadRenderizable& entity,
                              const int entity_x, const int entity_y, const int  cell_width,
                              const int cell_height, const int /*animation_row*/,
                              const int /*frame_index*/) const {
    const auto resolved = resolver_.resolved_creature(entity);
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;
    const int body_width = static_cast<int>((resolved.size.x + cell_width)* CHARACTER_SCALE);
    const int body_height = static_cast<int>((resolved.size.y+ cell_height)* CHARACTER_SCALE);
    const int body_x = anchor_x - body_width / 2 + resolved.offset.x;
    const int body_y = anchor_y - body_height + resolved.offset.y;

    renderer.Copy(*resolved.texture, to_sdl_rect(resolved.src),
                  SDL2pp::Rect(body_x, body_y, body_width, body_height));
}
