//
// Created by victoria zubieta on 02/06/2026.
//

#include "npc_sprite_resolver.h"

#include <stdexcept>
#include <string>

NpcSpriteResolver::NpcSpriteResolver(const SpriteCatalog& catalog, TextureCache& cache):
        catalog_(catalog),
        cache_(cache) {}

std::string NpcSpriteResolver::npc_key_from_entity(const EntidadRenderizable& entidad) const {
    switch (entidad.cuerpo) {
        case 4071: return "comerciante";
        case 1243: return "sacerdote";
        case 4057: return "banquero";
        default:
            throw std::runtime_error("NPC desconocido para cuerpo " +
                                     std::to_string(entidad.cuerpo));
    }
}

ResolvedNpcSprite NpcSpriteResolver::resolve(const EntidadRenderizable& entidad) const {
    const auto& npc_def = catalog_.npc(npc_key_from_entity(entidad));

    std::optional<ResolvedNpcPart> head;
    if (npc_def.head_path.has_value()) {
        head = ResolvedNpcPart{
                .texture = &cache_.get_or_load(*npc_def.head_path),
        };
    }

    std::optional<ResolvedNpcPart> body;
    if (npc_def.body_path.has_value()) {
        body = ResolvedNpcPart{
                .texture = &cache_.get_or_load(*npc_def.body_path),
        };
    }

    return ResolvedNpcSprite{
            .head = head,
            .body = body,
            .size = npc_def.size,
            .offset = npc_def.offset,
    };
}
