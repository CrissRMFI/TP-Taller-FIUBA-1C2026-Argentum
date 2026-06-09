//
// Created by victoria zubieta on 02/06/2026.
//

#include "npc_renderer.h"

SDL2pp::Rect NPCRenderer::to_sdl_rect(const SpriteRect& rect) const {
    return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
}

NPCRenderer::NPCRenderer(NpcSpriteResolver& resolver): resolver_(resolver) {}

void NPCRenderer::render(SDL2pp::Renderer& renderer, const Npc& npc,
                         const int entity_x, const int entity_y, const int cell_width,
                         const int cell_height, const int /*animation_row*/,
                         const int /*frame_index*/) const {
    const auto resolved = resolver_.resolve(npc);
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;
    const int body_width = resolved.size.x;
    const int body_height =  resolved.size.y;
    const int body_x = anchor_x - body_width / 2 + resolved.offset.x;
    const int body_y = anchor_y - body_height + resolved.offset.y;

    if (resolved.body.has_value()) {
        renderer.Copy(*resolved.body->texture, to_sdl_rect(resolved.src),
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
    }


    if (resolved.head.has_value()) {
        renderer.Copy(*resolved.head->texture, to_sdl_rect(resolved.src_head),
                      SDL2pp::Rect(body_x + 5, body_y + 1, body_width,body_height));
    }
}
