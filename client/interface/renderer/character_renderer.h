#ifndef TALLER_TP_CHARACTER_RENDERER_H
#define TALLER_TP_CHARACTER_RENDERER_H

#include "SDL2pp/Renderer.hh"
#include "client/entidad_renderizable.h"
#include "client/interface/sprites_resolver/character_sprite_resolver.h"

// dibujamos al personaje
class CharacterRenderer {
private:
    CharacterSpriteResolver& resolver_;
    SDL2pp::Rect to_sdl_rect(const SpriteRect& rect) const;
    SpriteRect body_src_rect_for(const CharacterPartDefinition& definition, int animation_row,
                                 int frame_index) const;
    SpriteRect head_src_rect_for(const CharacterPartDefinition& definition,
                                 int animation_row) const;


public:
    explicit CharacterRenderer(CharacterSpriteResolver& resolver);

    void render(SDL2pp::Renderer& renderer, const EntidadRenderizable& entity, int entity_x,
                int entity_y, int cell_width, int cell_height, int animation_row, int frame_index,
                bool resaltar = false) const;
};

#endif  // TALLER_TP_CHARACTER_RENDERER_H
