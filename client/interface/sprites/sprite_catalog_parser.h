//
// Created by victoria zubieta on 07/06/2026.
//

#ifndef TALLER_TP_SPRITE_CATALOG_PARSER_H
#define TALLER_TP_SPRITE_CATALOG_PARSER_H
#include "catalog_resources.h"
#include "sprite_catalog_helper.h"

class SpriteCatalogParser {
private:
    SpriteCatalogHelper helper;

public:
    CharacterPartDefinition parse_head_part(const toml::key& key, const toml::table& item,
                                            const std::string& path_prefix);
    CharacterPartDefinition parse_body_part(const toml::key& key, const toml::table& item,
                                            const std::string& path_prefix);
    std::unordered_map<std::string, SkinPreset> parse_skins(const toml::table& root);
    std::pair<std::unordered_map<uint16_t, CharacterPartDefinition>,
              std::unordered_map<uint16_t, CharacterPartDefinition>>
    parse_character_parts(const toml::table& root);
    std::unordered_map<std::string, StateOverride> parse_states(const toml::table& root);
    std::unordered_map<std::string, StaticSpriteDefinition> parse_creatures(
            const toml::table& root);
    std::unordered_map<std::string, NpcSpriteDefinition> parse_npcs(const toml::table& root);
    void validate_catalog(const std::unordered_map<std::string, SkinPreset>& skins,
                          const std::unordered_map<uint16_t, CharacterPartDefinition>& heads,
                          const std::unordered_map<uint16_t, CharacterPartDefinition>& bodies);
};


#endif  // TALLER_TP_SPRITE_CATALOG_PARSER_H
