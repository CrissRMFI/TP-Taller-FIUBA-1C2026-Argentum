//
// Created by victoria zubieta on 01/06/2026.
//

#ifndef TALLER_TP_CHARACTER_SPRITE_RESOLVER_H
#define TALLER_TP_CHARACTER_SPRITE_RESOLVER_H

#include "SDL2pp/Texture.hh"
#include "client/entidad_renderizable.h"
#include "client/interface/sprites/sprite_catalog.h"
#include "client/interface/texture_cache.h"
//  se encarga de armar la estructura del personaje
struct ResolvedCharacterPart {
    SDL2pp::Texture* texture;
    const CharacterPartDefinition* definition;
    std::optional<SpriteRect> src_override;
};

struct CharacterSprite {
    std::optional<ResolvedCharacterPart> head;
    std::optional<ResolvedCharacterPart> body;
};
class CharacterSpriteResolver {
private:
    const SpriteCatalog& sprite_catalog;
    TextureCache& texture_cache;

public:
    CharacterSpriteResolver(const SpriteCatalog& catalog, TextureCache& textures);
    CharacterSprite resolveSprite(const EntidadRenderizable& entity) const;
};


#endif  // TALLER_TP_CHARACTER_SPRITE_RESOLVER_H
