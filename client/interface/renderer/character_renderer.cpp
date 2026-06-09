#include "character_renderer.h"

#include <utility>

#include "SDL_render.h"

constexpr float CHARACTER_SCALE = 1.0f;
// constexpr int OFF_POSITION = 5;

SDL2pp::Rect CharacterRenderer::to_sdl_rect(const SpriteRect& rect) const {
    return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
}

SpriteRect CharacterRenderer::body_src_rect_for(const CharacterPartDefinition& definition,
                                                const int animation_row,
                                                const int frame_index) const {
    const auto& row = definition.rows.at(animation_row);
    if (!row.has_value()) {
        return definition.scr_body;
    }

    const int clamped_frame = row->frames > 0 ? frame_index % row->frames : 0;
    if (!definition.frame_size.has_value()) {
        return {
                clamped_frame * row->step_x + definition.scr_body.x,
                row->y + definition.scr_body.y,
                definition.scr_body.width,
                definition.scr_body.height};
    }

    const int frame_w = definition.frame_size->x;
    const int frame_h = definition.frame_size->y;
    return {
            clamped_frame * frame_w + row->step_x,
            row->row * frame_h + row->y,
            definition.scr_body.width,
            definition.scr_body.height};
}

SpriteRect CharacterRenderer::head_src_rect_for(const CharacterPartDefinition& definition,
                                                const int animation_row) const {
    const auto& direction = definition.directions.at(animation_row);
    if (!direction.has_value()) {
        return definition.scr_head;
    }

    if (!definition.frame_size.has_value()) {
        return {
                direction->src.x + definition.scr_head.x,
                direction->src.y + definition.scr_head.y,
                definition.scr_head.width,
                definition.scr_head.height};
    }

    const int frame_width = definition.frame_size->x;
    return {
            direction->column * frame_width + definition.scr_head.x,
            definition.scr_head.y,
            definition.scr_head.width,
            definition.scr_head.height};
}

CharacterRenderer::CharacterRenderer(CharacterSpriteResolver& resolver): resolver_(resolver) {}

void CharacterRenderer::render(SDL2pp::Renderer& renderer,
                               const EntidadRenderizable& entity,
                               const int entity_x,
                               const int entity_y,
                               const int cell_width,
                               const int cell_height,
                               const int animation_row,
                               const int frame_index,
                               const bool resaltar) const {
    const auto resolved = resolver_.resolveSprite(entity);
    const int effective_frame_index =
            (entity.estado == 1 || entity.estado == 3) ? 0 : frame_index;
    int body_x = entity_x;
    int body_y = entity_y;
    int body_width = cell_width;
    int body_height = cell_height;
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;

    if (resolved.body.has_value()) {
        // Para overrides estaticos (ej. fantasma) usamos el recorte fijo del sprite
        // y evitamos cualquier animacion basada en filas/frames del cuerpo original.
        const SpriteRect body_src = resolved.body->src_override.value_or(
                body_src_rect_for(*resolved.body->definition, animation_row,
                                  effective_frame_index));

        body_width = static_cast<int>(body_src.width * CHARACTER_SCALE);
        body_height = static_cast<int>(body_src.height * CHARACTER_SCALE);

        body_x = anchor_x - body_width / 2 + resolved.body->definition->draw_offset.x;
        body_y = anchor_y - body_height + resolved.body->definition->draw_offset.y;
        SDL_SetTextureBlendMode(resolved.body->texture->Get(), SDL_BLENDMODE_BLEND);

        // Resaltado: contorno verde de la silueta (cuerpo desplazado en 4 direcciones).
        if (resaltar) {
            SDL_SetTextureColorMod(resolved.body->texture->Get(), 80, 255, 120);
            const int o = 2;
            for (const auto& d : {std::pair{-o, 0}, std::pair{o, 0}, std::pair{0, -o},
                                  std::pair{0, o}}) {
                renderer.Copy(*resolved.body->texture, to_sdl_rect(body_src),
                              SDL2pp::Rect(body_x + d.first, body_y + d.second, body_width,
                                           body_height));
            }
            SDL_SetTextureColorMod(resolved.body->texture->Get(), 255, 255, 255);
        }
        const uint8_t alpha =
                (entity.estado == 1 || entity.estado == 2 || entity.estado == 3) ? 128 : 255;
        SDL_SetTextureAlphaMod(resolved.body->texture->Get(), alpha);
        renderer.Copy(*resolved.body->texture, to_sdl_rect(body_src),
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
        SDL_SetTextureAlphaMod(resolved.body->texture->Get(), 255);
    }

    // Overlays de arma/escudo: se dibujan en su celda completa (el item ya esta posicionado dentro del frame, p.ej. en la mano) alineada con el cuerpo por la base de la celda.
    const auto dibujar_overlay_cuerpo = [&](const ResolvedCharacterPart& part) {
        const SpriteRect ov_src =
                body_src_rect_for(*part.definition, animation_row, effective_frame_index);
        const int ov_w = static_cast<int>(ov_src.width * CHARACTER_SCALE);
        const int ov_h = static_cast<int>(ov_src.height * CHARACTER_SCALE);
        const int ov_x = anchor_x - ov_w / 2;
        const int ov_y = anchor_y - ov_h + 2;  // base de la celda ~ pies del personaje
        SDL_SetTextureBlendMode(part.texture->Get(), SDL_BLENDMODE_BLEND);
        renderer.Copy(*part.texture, to_sdl_rect(ov_src), SDL2pp::Rect(ov_x, ov_y, ov_w, ov_h));
    };
    if (resolved.escudo.has_value()) {
        dibujar_overlay_cuerpo(*resolved.escudo);
    }
    if (resolved.arma.has_value()) {
        dibujar_overlay_cuerpo(*resolved.arma);
    }
    if (resolved.head.has_value()) {
        const SpriteRect head_src =
                head_src_rect_for(*resolved.head->definition, animation_row);

        const int head_width = static_cast<int>(head_src.width * CHARACTER_SCALE);
        const int head_height = static_cast<int>(head_src.height * CHARACTER_SCALE);

        const int head_x = body_x + (body_width - head_width) / 2;

        const int head_y =
                body_y - head_height + resolved.head->definition->draw_offset.y;

        renderer.Copy(*resolved.head->texture, to_sdl_rect(head_src),
                      SDL2pp::Rect(head_x, head_y, head_width, head_height));

        // Casco: overlay sobre la cabeza (mismo formato/posicion que la cabeza).
        if (resolved.casco.has_value()) {
            const SpriteRect casco_src =
                    head_src_rect_for(*resolved.casco->definition, animation_row);
            const int casco_w = static_cast<int>(casco_src.width * CHARACTER_SCALE);
            const int casco_h = static_cast<int>(casco_src.height * CHARACTER_SCALE);
            const int casco_x = body_x + (body_width - casco_w) / 2;
            const int casco_y = body_y - casco_h + resolved.casco->definition->draw_offset.y;
            SDL_SetTextureBlendMode(resolved.casco->texture->Get(), SDL_BLENDMODE_BLEND);
            renderer.Copy(*resolved.casco->texture, to_sdl_rect(casco_src),
                          SDL2pp::Rect(casco_x, casco_y, casco_w, casco_h));
        }
    }
}
