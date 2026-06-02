#ifndef TALLER_TP_CHARACTER_RENDERER_H
#define TALLER_TP_CHARACTER_RENDERER_H

#include "SDL2pp/Renderer.hh"
#include "character_sprite_resolver.h"
#include "client/entidad_renderizable.h"

// dibujamos al personaje
class CharacterRenderer {
private:
    CharacterSpriteResolver& resolver_;

public:
    explicit CharacterRenderer(CharacterSpriteResolver& resolver);

    void render(SDL2pp::Renderer& renderer,
                const EntidadRenderizable& entity,
                int entity_x,
                int entity_y,
                int cell_width,
                int cell_height,
                int animation_row,
                int frame_index) const;
};

#endif  // TALLER_TP_CHARACTER_RENDERER_H
