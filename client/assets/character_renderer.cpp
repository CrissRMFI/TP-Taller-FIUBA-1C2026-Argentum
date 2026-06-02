#include "character_renderer.h"

#include "SDL_render.h"

namespace {

constexpr int FRONT_ROW = 0;
constexpr int FRONT_FRAME = 0;
constexpr float CHARACTER_SCALE = 1.0f;
constexpr  int OFF_POSITION = 5;

SDL2pp::Rect to_sdl_rect(const SpriteRect& rect) {
    return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
}

SpriteRect body_src_rect_for(const CharacterPartDefinition& definition, int animation_row,
                             int frame_index) {
    const auto& row = definition.rows.at(animation_row);
    if (!row.has_value()) {
        return {definition.visible_offset.x, definition.visible_offset.y,
                definition.visible_size.x, definition.visible_size.y};
    }

    const int clamped_frame = row->frames > 0 ? frame_index % row->frames : 0;
    return {
            clamped_frame * row->step_x + definition.visible_offset.x,
            row->y + definition.visible_offset.y,
            definition.visible_size.x,
            definition.visible_size.y};
}

SpriteRect head_src_rect_for(const CharacterPartDefinition& definition, int animation_row) {
    const auto& direction = definition.directions.at(animation_row);
    if (!direction.has_value()) {
        return {definition.visible_offset.x, definition.visible_offset.y,
                definition.visible_size.x, definition.visible_size.y};
    }
    return {
            direction->src.x + definition.visible_offset.x,
            direction->src.y + definition.visible_offset.y,
            definition.visible_size.x,
            definition.visible_size.y};
}

}  // namespace

CharacterRenderer::CharacterRenderer(CharacterSpriteResolver& resolver): resolver_(resolver) {}

void CharacterRenderer::render(SDL2pp::Renderer& renderer,
                               const EntidadRenderizable& entity,
                               const int entity_x,
                               const int entity_y,
                               const int cell_width,
                               const int cell_height,
                               const int /*animation_row*/,
                               const int /*frame_index*/) const {
    const auto resolved = resolver_.resolveSprite(entity);
    int body_x = entity_x;
    int body_y = entity_y;
    int body_width = cell_width;
    int body_height = cell_height;
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;

    if (resolved.body.has_value()) {
        const SpriteRect body_src =
                body_src_rect_for(*resolved.body->definition, FRONT_ROW, FRONT_FRAME);

        body_width = static_cast<int>(body_src.width * CHARACTER_SCALE);
        body_height = static_cast<int>(body_src.height * CHARACTER_SCALE);

        body_x = anchor_x - body_width / 2 + resolved.body->definition->draw_offset.x;
        body_y = anchor_y - body_height + resolved.body->definition->draw_offset.y;
        SDL_SetTextureBlendMode(resolved.body->texture->Get(), SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(resolved.body->texture->Get(), entity.estado == 1 ? 128 : 255);
        renderer.Copy(*resolved.body->texture, to_sdl_rect(body_src),
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
        SDL_SetTextureAlphaMod(resolved.body->texture->Get(), 255);
    }
    if (resolved.head.has_value()) {
        const SpriteRect head_src =
                head_src_rect_for(*resolved.head->definition, FRONT_ROW);

        const int head_width =
                static_cast<int>(resolved.head->definition->visible_size.x * CHARACTER_SCALE);
        const int head_height =
                static_cast<int>(resolved.head->definition->visible_size.y * CHARACTER_SCALE);

        const int head_x =  body_x + (body_width - head_width)/ 2;

        const int head_y = body_y - head_height + OFF_POSITION  + resolved.head->definition->draw_offset.y;

        renderer.Copy(*resolved.head->texture, to_sdl_rect(head_src),
                      SDL2pp::Rect(head_x, head_y, head_width, head_height));
    }
}
