//
// Created by victoria zubieta on 02/06/2026.
//

#ifndef TALLER_TP_CRIATURA_SPRITE_RESOLVER_H
#define TALLER_TP_CRIATURA_SPRITE_RESOLVER_H

#include "SDL2pp/Texture.hh"
#include "client/entidad_renderizable.h"
#include "client/interface/sprites/sprite_catalog.h"
#include "client/interface/texture_cache.h"

struct ResolvedCreatureSprite {
    SDL2pp::Texture* texture;
    SpriteRect src;
    SpriteVec2 size;
    SpriteVec2 offset;
};

class CreatureSpriteResolver {
private:
    const SpriteCatalog& catalog_;
    TextureCache& cache_;

public:
    CreatureSpriteResolver(const SpriteCatalog& catalog, TextureCache& cache);
    ResolvedCreatureSprite resolved_creature(const EntidadRenderizable& entidad) const;
};

#endif  // TALLER_TP_CRIATURA_SPRITE_RESOLVER_H
