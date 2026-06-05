//
// Created by victoria zubieta on 02/06/2026.
//

#ifndef TALLER_TP_NPC_SPRITE_RESOLVER_H
#define TALLER_TP_NPC_SPRITE_RESOLVER_H

#include <optional>
#include <string>

#include "SDL2pp/Texture.hh"
#include "common/game/npc/npc.h"
#include "sprite_catalog.h"
#include "texture_cache.h"

struct ResolvedNpcPart {
    SDL2pp::Texture* texture;
};

struct ResolvedNpcSprite {
    std::optional<ResolvedNpcPart> head;
    std::optional<ResolvedNpcPart> body;
    SpriteVec2 size;
    SpriteRect src_head;
    SpriteRect src;
    SpriteVec2 offset;
};

class NpcSpriteResolver {
private:
    const SpriteCatalog& catalog_;
    TextureCache& cache_;

public:
    NpcSpriteResolver(const SpriteCatalog& catalog, TextureCache& cache);
    std::string npc_key_from_npc(const Npc& npc) const;
    ResolvedNpcSprite resolve(const Npc& npc) const;
};

#endif  // TALLER_TP_NPC_SPRITE_RESOLVER_H
