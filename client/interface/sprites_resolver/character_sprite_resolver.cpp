//
// Created by victoria zubieta on 01/06/2026.
//

#include "character_sprite_resolver.h"

#include <iostream>

CharacterSpriteResolver::CharacterSpriteResolver(const SpriteCatalog& catalog,
                                                 TextureCache& textures) :
        sprite_catalog(catalog), texture_cache(textures) {}

CharacterSprite CharacterSpriteResolver::resolveSprite(const EntidadRenderizable& entity) const {
    std::optional<ResolvedCharacterPart> body;
    const bool has_body = sprite_catalog.has_body(entity.cuerpo);
    const bool has_head = sprite_catalog.has_head(entity.cabeza);

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
        std::cerr << "[sprite_resolver] body no encontrado para id=" << entity.cuerpo << std::endl;
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
                  << static_cast<int>(entity.estado) << " cabeza=" << entity.cabeza
                  << " has_head=" << has_head << std::endl;
    }
    // Overlays de vestimenta. Solo personajes vivos (no fantasma/resucitando).
    const bool puedeVestir = (entity.tipo == 0 && entity.estado != 1 && entity.estado != 3);
    const auto overlayCuerpo = [&](uint16_t id) -> std::optional<ResolvedCharacterPart> {
        if (!puedeVestir || id == 0 || !sprite_catalog.has_body(id)) {
            return std::nullopt;
        }
        const auto& def = sprite_catalog.body(id);
        return ResolvedCharacterPart{
                .texture = &texture_cache.get_or_load(def.path),
                .definition = &def,
                .src_override = std::nullopt,
        };
    };

    std::optional<ResolvedCharacterPart> arma = overlayCuerpo(entity.arma);
    std::optional<ResolvedCharacterPart> escudo = overlayCuerpo(entity.escudo);

    std::optional<ResolvedCharacterPart> casco;
    if (puedeVestir && entity.casco != 0 && sprite_catalog.has_head(entity.casco)) {
        const auto& casco_def = sprite_catalog.head(entity.casco);
        casco = ResolvedCharacterPart{
                .texture = &texture_cache.get_or_load(casco_def.path),
                .definition = &casco_def,
                .src_override = std::nullopt,
        };
    }

    return CharacterSprite{head, body, arma, escudo, casco};
}
