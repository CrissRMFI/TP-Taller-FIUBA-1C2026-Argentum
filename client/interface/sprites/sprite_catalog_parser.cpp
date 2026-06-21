//
// Created by victoria zubieta on 07/06/2026.
//

#include "sprite_catalog_parser.h"
#include <toml++/toml.hpp>

CharacterPartDefinition SpriteCatalogParser::parse_head_part(const toml::key& key,
                                                             const toml::table& item,
                                                             const std::string& path_prefix) {
    CharacterPartDefinition part{
            .id = helper.require_uint16_from_key(key, path_prefix),
            .path = helper.require_string(item, "path", helper.join_path(path_prefix, "path")),
            .scr_head = helper.require_rect4(item, "src_head",
                                             helper.join_path(path_prefix, "src_head")),
            .scr_body = SpriteRect{0, 0, 0, 0},
            .frame_size = helper.require_vec2(item, "frame_size",
                                              helper.join_path(path_prefix, "frame_size")),
            .draw_offset =
                    helper.optional_vec2_or_default(item, "draw_offset", SpriteVec2{0, 0},
                                                    helper.join_path(path_prefix, "draw_offset")),
            .directions = {},
            .rows = {},
    };

    const toml::table& directions = helper.require_subtable(
            item, "directions", helper.join_path(path_prefix, "directions"));
    for (const auto& [direction_key, direction_value] : directions) {
        const std::string direction_name(direction_key.str());
        const std::string direction_path =
                helper.join_path(helper.join_path(path_prefix, "directions"), direction_name);
        const toml::table* direction_table = direction_value.as_table();
        if (!direction_table) {
            helper.throw_invalid(direction_path, "se esperaba una tabla");
        }
        const int column =
                helper.require_int_any_key(*direction_table, {"column", "col"},
                                           helper.join_path(direction_path, "column|col"));
        const SpriteRect fallback_src = {
                column * part.frame_size->x,
                0,
                part.frame_size->x,
                part.frame_size->y,
        };
        auto index = helper.row_index_for_key(direction_name);
        part.directions[index] = SpriteSheetDirection{
                .column = column,
                .src = helper.optional_rect4(*direction_table, "src",
                                             helper.join_path(direction_path, "src"))
                               .value_or(fallback_src),
        };
    }

    helper.validate_directions(part, path_prefix);
    return part;
}

CharacterPartDefinition SpriteCatalogParser::parse_body_part(const toml::key& key,
                                                             const toml::table& item,
                                                             const std::string& path_prefix) {
    CharacterPartDefinition part{
            .id = helper.require_uint16_from_key(key, path_prefix),
            .path = helper.require_string(item, "path", helper.join_path(path_prefix, "path")),
            .scr_head = SpriteRect{0, 0, 0, 0},
            .scr_body = helper.require_rect4(item, "src_body",
                                             helper.join_path(path_prefix, "src_body")),
            .frame_size = helper.require_vec2(item, "frame_size",
                                              helper.join_path(path_prefix, "frame_size")),
            .draw_offset =
                    helper.optional_vec2_or_default(item, "draw_offset", SpriteVec2{0, 0},
                                                    helper.join_path(path_prefix, "draw_offset")),
            .directions = {},
            .rows = {},
    };

    const toml::table& rows =
            helper.require_subtable(item, "rows", helper.join_path(path_prefix, "rows"));
    for (const auto& [row_key, row_value] : rows) {
        const std::string row_name(row_key.str());
        const std::string row_path =
                helper.join_path(helper.join_path(path_prefix, "rows"), row_name);
        const toml::table* row_table = row_value.as_table();
        if (!row_table) {
            helper.throw_invalid(row_path, "se esperaba una tabla");
        }
        auto index = helper.row_index_for_key(row_name);
        part.rows[index] = SpriteSheetAnimationRow{
                .y = helper.require_int(*row_table, "y", helper.join_path(row_path, "y")),
                .frames = helper.require_int(*row_table, "frames",
                                             helper.join_path(row_path, "frames")),
                .step_x = helper.require_int(*row_table, "step_x",
                                             helper.join_path(row_path, "step_x")),
                .row = helper.require_int(*row_table, "row", helper.join_path(row_path, "row")),
        };
    }

    helper.validate_rows(part, path_prefix);
    return part;
}

std::unordered_map<std::string, SkinPreset> SpriteCatalogParser::parse_skins(
        const toml::table& root) {
    std::unordered_map<std::string, SkinPreset> skins_map;
    const toml::table& skins_table = helper.require_subtable(root, "skins", "skins");
    for (const auto& [key, value] : skins_table) {
        const std::string skin_name(key.str());
        const toml::table* skin_table = value.as_table();
        if (!skin_table) {
            helper.throw_invalid(helper.join_path("skins", skin_name), "se esperaba una tabla");
        }

        skins_map.emplace(
                skin_name,
                SkinPreset{
                        .head_ids = helper.require_uint16_array(
                                *skin_table, "head_ids",
                                helper.join_path(helper.join_path("skins", skin_name), "head_ids")),
                        .body_ids = helper.require_uint16_array(
                                *skin_table, "body_ids",
                                helper.join_path(helper.join_path("skins", skin_name), "body_ids")),
                });
    }
    return skins_map;
}

std::pair<std::unordered_map<uint16_t, CharacterPartDefinition>,
          std::unordered_map<uint16_t, CharacterPartDefinition>>
SpriteCatalogParser::parse_character_parts(const toml::table& root) {
    std::unordered_map<uint16_t, CharacterPartDefinition> heads_map;
    std::unordered_map<uint16_t, CharacterPartDefinition> bodies_map;
    const toml::table& character_parts =
            helper.require_subtable(root, "character_parts", "character_parts");
    const toml::table& heads =
            helper.require_subtable(character_parts, "heads", "character_parts.heads");
    const toml::table& bodies =
            helper.require_subtable(character_parts, "bodies", "character_parts.bodies");

    for (const auto& [key, value] : heads) {
        const std::string path_prefix =
                helper.join_path("character_parts.heads", std::string(key.str()));
        const toml::table* item = value.as_table();
        if (!item) {
            helper.throw_invalid(path_prefix, "se esperaba una tabla");
        }
        const auto part = parse_head_part(key, *item, path_prefix);
        heads_map.emplace(part.id, part);
    }

    for (const auto& [key, value] : bodies) {
        const std::string path_prefix =
                helper.join_path("character_parts.bodies", std::string(key.str()));
        const toml::table* item = value.as_table();
        if (!item) {
            helper.throw_invalid(path_prefix, "se esperaba una tabla");
        }
        const auto part = parse_body_part(key, *item, path_prefix);
        bodies_map.emplace(part.id, part);
    }
    return {std::move(heads_map), std::move(bodies_map)};
}

std::unordered_map<std::string, StateOverride> SpriteCatalogParser::parse_states(
        const toml::table& root) {
    std::unordered_map<std::string, StateOverride> states_map;
    if (const toml::table* states = root["states"].as_table()) {
        for (const auto& [key, value] : *states) {
            const std::string state_name(key.str());
            const std::string path_prefix = helper.join_path("states", state_name);
            const toml::table* item = value.as_table();
            if (!item) {
                helper.throw_invalid(path_prefix, "se esperaba una tabla");
            }

            StateOverride state;
            if (auto head_path = item->get_as<std::string>("head_path")) {
                state.head_path = head_path->get();
            }
            if (auto body_path = item->get_as<std::string>("body_path")) {
                state.body_path = body_path->get();
            }
            state.body_src =
                    helper.optional_rect4(*item, "src", helper.join_path(path_prefix, "src"));
            if (!state.head_path.has_value() && !state.body_path.has_value()) {
                helper.throw_invalid(path_prefix, "debe definir head_path o body_path");
            }

            states_map.emplace(state_name, std::move(state));
        }
    }
    return states_map;
}

std::unordered_map<std::string, StaticSpriteDefinition> SpriteCatalogParser::parse_creatures(
        const toml::table& root) {
    std::unordered_map<std::string, StaticSpriteDefinition> creatures_map;
    if (const toml::table* creatures = root["creatures"].as_table()) {
        for (const auto& [key, value] : *creatures) {
            const std::string creature_name(key.str());
            const std::string path_prefix = helper.join_path("creatures", creature_name);
            const toml::table* item = value.as_table();
            if (!item) {
                helper.throw_invalid(path_prefix, "se esperaba una tabla");
            }

            creatures_map.emplace(
                    creature_name,
                    StaticSpriteDefinition{
                            .id = static_cast<uint16_t>(helper.require_int(
                                    *item, "id", helper.join_path(path_prefix, "id"))),
                            .part = helper.require_string(*item, "part",
                                                          helper.join_path(path_prefix, "part")),
                            .path = helper.require_string(*item, "path",
                                                          helper.join_path(path_prefix, "path")),
                            .src = helper.require_rect4(*item, "src",
                                                        helper.join_path(path_prefix, "src")),
                            .size = helper.require_vec2(*item, "size",
                                                        helper.join_path(path_prefix, "size")),
                            .offset = helper.require_vec2(*item, "offset",
                                                          helper.join_path(path_prefix, "offset")),
                    });
        }
    }
    return creatures_map;
}

std::unordered_map<std::string, NpcSpriteDefinition> SpriteCatalogParser::parse_npcs(
        const toml::table& root) {
    std::unordered_map<std::string, NpcSpriteDefinition> npcs_map;
    if (const toml::table* npcs = root["npcs"].as_table()) {
        for (const auto& [key, value] : *npcs) {
            const std::string npc_name(key.str());
            const std::string path_prefix = helper.join_path("npcs", npc_name);
            const toml::table* item = value.as_table();
            if (!item) {
                helper.throw_invalid(path_prefix, "se esperaba una tabla");
            }

            NpcSpriteDefinition npc;
            if (auto head_path = item->get_as<std::string>("head_path")) {
                npc.head_path = head_path->get();
            }
            if (auto body_path = item->get_as<std::string>("body_path")) {
                npc.body_path = body_path->get();
            }
            npc.size = helper.optional_vec2_or_default(*item, "size", SpriteVec2{17, 40},
                                                       helper.join_path(path_prefix, "size"));
            npc.src_head = helper.require_rect4(*item, "src", helper.join_path(path_prefix, "src")),
            npc.src = helper.require_rect4(*item, "src", helper.join_path(path_prefix, "src")),
            npc.offset = helper.optional_vec2_or_default(*item, "offset", SpriteVec2{0, 0},
                                                         helper.join_path(path_prefix, "offset"));
            if (!npc.head_path.has_value() && !npc.body_path.has_value()) {
                helper.throw_invalid(path_prefix, "debe definir head_path o body_path");
            }

            npcs_map.emplace(npc_name, std::move(npc));
        }
    }
    return npcs_map;
}

void SpriteCatalogParser::validate_catalog(
        const std::unordered_map<std::string, SkinPreset>& skins,
        const std::unordered_map<uint16_t, CharacterPartDefinition>& heads,
        const std::unordered_map<uint16_t, CharacterPartDefinition>& bodies) {
    for (const auto& [skin_name, preset] : skins) {
        helper.validate_skin_parts_exist(skin_name, preset, heads, bodies);
    }
}
