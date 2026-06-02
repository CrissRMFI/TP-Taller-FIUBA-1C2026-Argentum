//
// Created by victoria zubieta on 02/06/2026.
//

#include "npc_renderer.h"


NPCRenderer::NPCRenderer(NpcSpriteResolver& resolver): resolver_(resolver) {}

void NPCRenderer::render(SDL2pp::Renderer& renderer, const EntidadRenderizable& entity,
                         const int entity_x, const int entity_y, const int cell_width,
                         const int cell_height, const int /*animation_row*/,
                         const int /*frame_index*/) const {
    const auto resolved = resolver_.resolve(entity);
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;
    const int body_width = resolved.size.x;
    const int body_height = resolved.size.y;
    const int body_x = anchor_x - body_width / 2 ;
    const int body_y = anchor_y - body_height ;

    if (resolved.body.has_value()) {
        renderer.Copy(*resolved.body->texture, SDL2pp::NullOpt,
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
    }

    if (resolved.head.has_value()) {
        renderer.Copy(*resolved.head->texture, SDL2pp::NullOpt,
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
    }
}
