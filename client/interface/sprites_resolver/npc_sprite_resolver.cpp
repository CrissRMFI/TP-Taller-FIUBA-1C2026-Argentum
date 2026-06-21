//
// Created by victoria zubieta on 02/06/2026.
//

#include "npc_sprite_resolver.h"

#include <stdexcept>
#include <string>

NpcSpriteResolver::NpcSpriteResolver(const SpriteCatalog& catalog, TextureCache& cache) :
        catalog_(catalog), cache_(cache) {}

std::string NpcSpriteResolver::npc_key_from_npc(const Npc& npc) const {
    switch (npc.getTipo()) {
        case TipoNpc::Comerciante:
            return "comerciante";
        case TipoNpc::Sacerdote:
            return "sacerdote";
        case TipoNpc::Banquero:
            return "banquero";
        default:
            throw std::runtime_error("NPC desconocido para tipo");
    }
}

ResolvedNpcSprite NpcSpriteResolver::resolve(const Npc& npc) const {
    const auto& npc_def = catalog_.npc(npc_key_from_npc(npc));

    // std::optional<ResolvedNpcPart> head;
    // if (npc_def.head_path.has_value()) {
    //     head = ResolvedNpcPart{
    //             .texture = &cache_.get_or_load(*npc_def.head_path),
    //     };
    // }

    std::optional<ResolvedNpcPart> body;
    if (npc_def.body_path.has_value()) {
        body = ResolvedNpcPart{
                .texture = &cache_.get_or_load(*npc_def.body_path),
        };
    }

    return ResolvedNpcSprite{
            // .head = head,
            .body = body,
            .size = npc_def.size,
            // .src_head = npc_def.src_head,
            .src = npc_def.src,
            .offset = npc_def.offset,
    };
}
