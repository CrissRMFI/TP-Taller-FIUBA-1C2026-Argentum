//
// Created by victoria zubieta on 02/06/2026.
//

#include "criatura_renderer.h"

#include <utility>

#include "SDL_render.h"


constexpr float CHARACTER_SCALE = 1.0f;

SDL2pp::Rect CriaturaRenderer::to_sdl_rect(const SpriteRect& rect) const {
    return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
}

CriaturaRenderer::CriaturaRenderer(CreatureSpriteResolver& resolver):
        resolver_(resolver) {}
// tengo que ajustar el tamaño de corte
void CriaturaRenderer::render(SDL2pp::Renderer& renderer, const EntidadRenderizable& entity,
                              const int entity_x, const int entity_y, const int  cell_width,
                              const int cell_height, const int /*animation_row*/,
                              const int /*frame_index*/, const bool resaltar) const {
    const auto resolved = resolver_.resolved_creature(entity);
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;
    const int body_width = std::max(1, static_cast<int>(resolved.size.x * CHARACTER_SCALE));
    const int body_height = std::max(1, static_cast<int>(resolved.size.y * CHARACTER_SCALE));
    const int body_x = anchor_x - body_width / 2 + resolved.offset.x;
    const int body_y = anchor_y - body_height + resolved.offset.y;
    const SDL2pp::Rect src = to_sdl_rect(resolved.src);

    // Resaltado: contorno de la figura (la silueta dibujada en verde desplazada en 4 dirs).
    if (resaltar) {
        SDL_SetTextureColorMod(resolved.texture->Get(), 80, 255, 120);
        const int o = 2;
        for (const auto& d : {std::pair{-o, 0}, std::pair{o, 0}, std::pair{0, -o}, std::pair{0, o}}) {
            renderer.Copy(*resolved.texture, src,
                          SDL2pp::Rect(body_x + d.first, body_y + d.second, body_width, body_height));
        }
        SDL_SetTextureColorMod(resolved.texture->Get(), 255, 255, 255);
    }

    renderer.Copy(*resolved.texture, src,
                  SDL2pp::Rect(body_x, body_y, body_width, body_height));
}
