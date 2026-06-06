//
// Created by victoria zubieta on 01/06/2026.
//

#include "character_sprite_resolver.h"

#include "criatura_renderer.h"

CharacterSpriteResolver::CharacterSpriteResolver(const SpriteCatalog& catalog,
                                                 TextureCache& textures):
sprite_catalog(catalog), texture_cache(textures){}

CharacterSprite CharacterSpriteResolver::resolveSprite(const EntidadRenderizable& entity) const {
    std::optional<ResolvedCharacterPart> body;

    // Sin un cuerpo valido en el catalogo no se resuelve el sprite del cuerpo: se deja el body vacio (el renderer lo omite) en lugar de llamar a body(id) y que unordered_map::at lance. 
    if (sprite_catalog.has_body(entity.cuerpo)) {
        if (entity.estado == 1 || entity.estado == 3) {
            if (const StateOverride* ghost_state = sprite_catalog.state_override("fantasma");
                ghost_state && ghost_state->body_path.has_value()) {
                const auto& base_body_def = sprite_catalog.body(entity.cuerpo);
                body = ResolvedCharacterPart{
                        .texture = &texture_cache.get_or_load(*ghost_state->body_path),
                        .definition = &base_body_def,
                };
            }
        }

        if (!body.has_value()) {
            const auto& body_def = sprite_catalog.body(entity.cuerpo);
            body = ResolvedCharacterPart{
                    .texture = &texture_cache.get_or_load(body_def.path),
                    .definition = &body_def,
            };
        }
    }

    std::optional<ResolvedCharacterPart> head;
    if (entity.tipo == 0 && entity.estado != 1 && entity.estado != 3 && entity.cabeza != 0 &&
        sprite_catalog.has_head(entity.cabeza)) {
        const auto& head_def = sprite_catalog.head(entity.cabeza);
        head = ResolvedCharacterPart{
                .texture = &texture_cache.get_or_load(head_def.path),
                .definition = &head_def,
        };
    }
    return CharacterSprite{head, body};
}
