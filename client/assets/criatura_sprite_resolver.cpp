//
// Created by victoria zubieta on 02/06/2026.
//

#include "criatura_sprite_resolver.h"

#include <stdexcept>
#include <string>

CreatureSpriteResolver::CreatureSpriteResolver(const SpriteCatalog& catalog,
                                               TextureCache& cache):
        catalog_(catalog),
        cache_(cache) {}

std::string CreatureSpriteResolver::creature_key_from_entity(
        const EntidadRenderizable& entidad) const {
    switch (entidad.cuerpo) {
        case 2200: return "goblin";
        case 2201: return "esqueleto";
        case 2202: return "zombie";
        case 2203: return "arana";
        case 2204: return "orco";
        case 2205: return "golem";
        default:
            throw std::runtime_error("Criatura desconocida para cuerpo " +
                                     std::to_string(entidad.cuerpo));
    }
}

ResolvedCreatureSprite CreatureSpriteResolver::resolved_creature(const EntidadRenderizable& entidad) const {
    const std::string creature_key = creature_key_from_entity(entidad);
    const auto& body_def = catalog_.creature(creature_key);

    return ResolvedCreatureSprite{
        .texture = &cache_.get_or_load(body_def.path),
        .src = body_def.src,
        .size = body_def.size,
        .offset = body_def.offset,
    };
}
