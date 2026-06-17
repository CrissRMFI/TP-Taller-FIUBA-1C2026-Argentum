#include "npc_renderer.h"

#include <utility>

#include "SDL_render.h"

SDL2pp::Rect NPCRenderer::to_sdl_rect(const SpriteRect& rect) const {
    return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
}
NPCRenderer::NPCRenderer(NpcSpriteResolver& resolver): resolver_(resolver) {}

void NPCRenderer::render(SDL2pp::Renderer& renderer, const Npc& npc,
                         const int entity_x, const int entity_y, const int cell_width,
                         const int cell_height, const int /*animation_row*/,
                         const int /*frame_index*/, const bool resaltar) const {
    const auto resolved = resolver_.resolve(npc);
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;
    const int body_width = resolved.size.x;
    const int body_height =  resolved.size.y;
    const int body_x = anchor_x - body_width / 2 + resolved.offset.x;
    const int body_y = anchor_y - body_height + resolved.offset.y;

    if (resolved.body.has_value()) {
        // Resaltado (seleccion / hover)
        if (resaltar) {
            SDL_SetTextureColorMod(resolved.body->texture->Get(), 80, 255, 120);
            const int o = 2;
            for (const auto& d : {std::pair{-o, 0}, std::pair{o, 0}, std::pair{0, -o},
                                  std::pair{0, o}}) {
                renderer.Copy(*resolved.body->texture, to_sdl_rect(resolved.src),
                              SDL2pp::Rect(body_x + d.first, body_y + d.second, body_width,
                                           body_height));
            }
            SDL_SetTextureColorMod(resolved.body->texture->Get(), 255, 255, 255);
        }
        renderer.Copy(*resolved.body->texture, to_sdl_rect(resolved.src),
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
    }
}
