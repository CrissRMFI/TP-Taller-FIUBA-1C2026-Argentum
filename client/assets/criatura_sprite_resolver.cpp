//
// Created by victoria zubieta on 02/06/2026.
//

#include "criatura_sprite_resolver.h"

CreatureSpriteResolver::CreatureSpriteResolver(const SpriteCatalog& catalog,
                                               TextureCache& cache):
        catalog_(catalog),
        cache_(cache) {}

ResolvedCreatureSprite CreatureSpriteResolver::resolved_creature(const EntidadRenderizable& entidad) const {
    const auto& body_def = catalog_.creature(entidad.cuerpo);

    return ResolvedCreatureSprite{
        .texture = &cache_.get_or_load(body_def.path),
        .src = body_def.src,
        .size = body_def.size,
        .offset = body_def.offset,
    };
}
