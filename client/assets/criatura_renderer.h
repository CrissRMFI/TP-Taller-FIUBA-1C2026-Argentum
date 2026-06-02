//
// Created by victoria zubieta on 02/06/2026.
//

#ifndef TALLER_TP_CRIATURA_RENDERER_H
#define TALLER_TP_CRIATURA_RENDERER_H


#include "criatura_sprite_resolver.h"


class CriaturaRenderer {
private:
    CreatureSpriteResolver& resolver_;

public:
    explicit CriaturaRenderer(CreatureSpriteResolver& resolver);

    void render(SDL2pp::Renderer& renderer,
                const EntidadRenderizable& entity,
                int entity_x,
                int entity_y,
                int cell_width,
                int cell_height,
                int animation_row,
                int frame_index) const;
};


#endif  // TALLER_TP_CRIATURA_RENDERER_H
