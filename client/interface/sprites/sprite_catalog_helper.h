//
// Created by victoria zubieta on 06/06/2026.
//

#ifndef TALLER_TP_SPRITE_CATALOG_HELPER_H
#define TALLER_TP_SPRITE_CATALOG_HELPER_H
#include <toml++/toml.hpp>
#include <unordered_map>
#include <vector>
#include "catalog_resources.h"



class SpriteCatalogHelper {
    public:
        std::string join_path(const std::string& a, const std::string& b);
        int row_index_for_key(const std::string& key);
        void throw_missing(const std::string& path);
        void throw_invalid(const std::string& path, const std::string& detail);
        const toml::table& require_subtable(const toml::table& table, const std::string& key,
                                            const std::string& path);
        std::string require_string(const toml::table& table, const std::string& key,
                                   const std::string& path);
        int require_int(const toml::table& table, const std::string& key, const std::string& path);
        int require_int_any_key(const toml::table& table,
                                const std::initializer_list<std::string>& keys,
                                const std::string& path);
        uint16_t require_uint16_from_key(const toml::key& key, const std::string& path);
        std::vector<uint16_t> require_uint16_array(const toml::table& table, const std::string& key,
                                                   const std::string& path);
        SpriteVec2 require_vec2(const toml::table& table, const std::string& key,
                                const std::string& path);
        SpriteVec2 optional_vec2_or_default(const toml::table& table, const std::string& key,
                                            SpriteVec2 default_value, const std::string& path);
        SpriteRect require_rect4(const toml::table& table, const std::string& key,
                                 const std::string& path);
        std::optional<SpriteRect> optional_rect4(const toml::table& table, const std::string& key,
                                                 const std::string& path);
        void validate_skin_parts_exist(
                const std::string& skin_name, const SkinPreset& preset,
                const std::unordered_map<uint16_t, CharacterPartDefinition>& heads,
                const std::unordered_map<uint16_t, CharacterPartDefinition>& bodies);
        void validate_rows(const CharacterPartDefinition& part, const std::string& path);
        void validate_directions(const CharacterPartDefinition& part, const std::string& path);
};


#endif  // TALLER_TP_SPRITE_CATALOG_HELPER_H
