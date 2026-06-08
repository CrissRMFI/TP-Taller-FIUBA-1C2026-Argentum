//
// Created by victoria zubieta on 01/06/2026.
//

#include "character_sprite_resolver.h"

#include <iostream>

CharacterSpriteResolver::CharacterSpriteResolver(const SpriteCatalog& catalog,
                                                 TextureCache& textures):
sprite_catalog(catalog), texture_cache(textures){}

CharacterSprite CharacterSpriteResolver::resolveSprite(const EntidadRenderizable& entity) const {
    std::optional<ResolvedCharacterPart> body;
    const bool has_body = sprite_catalog.has_body(entity.cuerpo);
    const bool has_head = sprite_catalog.has_head(entity.cabeza);

    // std::cerr << "[sprite_resolver] entity tipo=" << static_cast<int>(entity.tipo)
    //           << " estado=" << static_cast<int>(entity.estado)
    //           << " cabeza=" << entity.cabeza
    //           << " cuerpo=" << entity.cuerpo
    //           << " has_head=" << has_head
    //           << " has_body=" << has_body << std::endl;

    // Sin un cuerpo valido en el
    // catalogo no se resuelve el sprite del cuerpo: se deja el body vacio (el renderer lo omite) e
    // n lugar de llamar a body(id) y que unordered_map::at lance.
    if (has_body) {
        if (entity.estado == 1 || entity.estado == 3) {
            if (const StateOverride* ghost_state = sprite_catalog.state_override("fantasma");
                ghost_state && ghost_state->body_path.has_value()) {
                const auto& base_body_def = sprite_catalog.body(entity.cuerpo);
                body = ResolvedCharacterPart{
                        .texture = &texture_cache.get_or_load(*ghost_state->body_path),
                        .definition = &base_body_def,
                        .src_override = ghost_state->body_src,
                };
            }
        }

        if (!body.has_value()) {
            const auto& body_def = sprite_catalog.body(entity.cuerpo);
            body = ResolvedCharacterPart{
                    .texture = &texture_cache.get_or_load(body_def.path),
                    .definition = &body_def,
                    .src_override = std::nullopt,
            };
        }
    } else {
        std::cerr << "[sprite_resolver] body no encontrado para id=" << entity.cuerpo
                  << std::endl;
    }

    std::optional<ResolvedCharacterPart> head;
    if (entity.tipo == 0 && entity.estado != 1 && entity.estado != 3 && entity.cabeza != 0 &&
        has_head) {
        const auto& head_def = sprite_catalog.head(entity.cabeza);
        head = ResolvedCharacterPart{
                .texture = &texture_cache.get_or_load(head_def.path),
                .definition = &head_def,
                .src_override = std::nullopt,
        };
    } else if (entity.tipo == 0) {
        std::cerr << "[sprite_resolver] head omitida para entidad: estado="
                  << static_cast<int>(entity.estado)
                  << " cabeza=" << entity.cabeza
                  << " has_head=" << has_head << std::endl;
    }
    return CharacterSprite{head, body};
}
