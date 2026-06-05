#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <toml++/toml.hpp>

#include "sprite_catalog.h"

namespace {

std::string join_path(const std::string& a, const std::string& b) {
    if (a.empty()) {
        return b;
    }
    return a + "." + b;
}

int row_index_for_key(const std::string& key) {
    static const std::unordered_map<std::string, int> mapping = {
            {"down", 0},
            {"left", 1},
            {"right", 2},
            {"up", 3},
    };
    const auto it = mapping.find(key);
    if (it == mapping.end()) {
        throw std::runtime_error("Config TOML invalida en " + key + ": direccion desconocida");
    }
    return it->second;
}

[[noreturn]] void throw_missing(const std::string& path) {
    throw std::runtime_error("Falta config TOML obligatoria: " + path);
}

[[noreturn]] void throw_invalid(const std::string& path, const std::string& detail) {
    throw std::runtime_error("Config TOML invalida en " + path + ": " + detail);
}

const toml::table& require_subtable(const toml::table& table, const std::string& key,
                                    const std::string& path) {
    const toml::table* child = table[key].as_table();
    if (!child) {
        throw_missing(path);
    }
    return *child;
}

std::string require_string(const toml::table& table, const std::string& key,
                           const std::string& path) {
    auto value = table[key].value<std::string>();
    if (!value.has_value() || value->empty()) {
        throw_missing(path);
    }
    return *value;
}

int require_int(const toml::table& table, const std::string& key, const std::string& path) {
    auto value = table[key].value<int64_t>();
    if (!value.has_value()) {
        throw_missing(path);
    }
    return static_cast<int>(*value);
}

uint16_t require_uint16_from_key(const toml::key& key, const std::string& path) {
    const std::string raw = std::string(key.str());
    try {
        const unsigned long parsed = std::stoul(raw);
        if (parsed > std::numeric_limits<uint16_t>::max()) {
            throw_invalid(path, "id fuera de rango uint16");
        }
        return static_cast<uint16_t>(parsed);
    } catch (const std::exception&) {
        throw_invalid(path, "la clave no es un id numerico");
    }
    return 0;
}

std::vector<uint16_t> require_uint16_array(const toml::table& table, const std::string& key,
                                           const std::string& path) {
    const toml::array* array = table[key].as_array();
    if (!array || array->empty()) {
        throw_missing(path);
    }

    std::vector<uint16_t> values;
    values.reserve(array->size());
    for (size_t index = 0; index < array->size(); ++index) {
        const auto value = (*array)[index].value<int64_t>();
        if (!value.has_value() || *value < 0 ||
            *value > std::numeric_limits<uint16_t>::max()) {
            throw_invalid(path, "array de ids invalido");
        }
        values.push_back(static_cast<uint16_t>(*value));
    }
    return values;
}

SpriteVec2 require_vec2(const toml::table& table, const std::string& key,
                        const std::string& path) {
    const toml::array* array = table[key].as_array();
    if (!array || array->size() != 2) {
        throw_invalid(path, "se esperaban 2 enteros");
    }

    const auto x = (*array)[0].value<int64_t>();
    const auto y = (*array)[1].value<int64_t>();
    if (!x.has_value() || !y.has_value()) {
        throw_invalid(path, "se esperaban 2 enteros");
    }

    return {static_cast<int>(*x), static_cast<int>(*y)};
}

SpriteVec2 optional_vec2_or_default(const toml::table& table, const std::string& key,
                                    const SpriteVec2 default_value,
                                    const std::string& path) {
    if (!table.contains(key)) {
        return default_value;
    }
    return require_vec2(table, key, path);
}

SpriteRect require_rect4(const toml::table& table, const std::string& key,
                         const std::string& path) {
    const toml::array* array = table[key].as_array();
    if (!array || array->size() != 4) {
        throw_invalid(path, "se esperaban 4 enteros");
    }

    const auto x = (*array)[0].value<int64_t>();
    const auto y = (*array)[1].value<int64_t>();
    const auto w = (*array)[2].value<int64_t>();
    const auto h = (*array)[3].value<int64_t>();
    if (!x.has_value() || !y.has_value() || !w.has_value() || !h.has_value()) {
        throw_invalid(path, "se esperaban 4 enteros");
    }

    return {static_cast<int>(*x), static_cast<int>(*y), static_cast<int>(*w),
            static_cast<int>(*h)};
}

void validate_skin_parts_exist(const std::string& skin_name, const SkinPreset& preset,
                               const std::unordered_map<uint16_t, CharacterPartDefinition>& heads,
                               const std::unordered_map<uint16_t, CharacterPartDefinition>& bodies) {
    for (const uint16_t head_id : preset.head_ids) {
        if (!heads.contains(head_id)) {
            throw_invalid(join_path("skins", skin_name),
                          "head_id " + std::to_string(head_id) +
                                  " no existe en character_parts.heads");
        }
    }
    for (const uint16_t body_id : preset.body_ids) {
        if (!bodies.contains(body_id)) {
            throw_invalid(join_path("skins", skin_name),
                          "body_id " + std::to_string(body_id) +
                                  " no existe en character_parts.bodies");
        }
    }
}

void validate_rows(const CharacterPartDefinition& part, const std::string& path) {
    for (int index = 0; index < 4; ++index) {
        if (!part.rows[index].has_value()) {
            throw_invalid(path, "faltan filas de animacion");
        }
        if (part.rows[index]->frames <= 0 || part.rows[index]->step_x <= 0) {
            throw_invalid(path, "frames y step_x deben ser positivos");
        }
    }
}

void validate_directions(const CharacterPartDefinition& part, const std::string& path) {
    for (int index = 0; index < 4; ++index) {
        if (!part.directions[index].has_value()) {
            throw_invalid(path, "faltan direcciones");
        }
    }
}

CharacterPartDefinition parse_head_part(const toml::key& key, const toml::table& item,
                                        const std::string& path_prefix) {
    CharacterPartDefinition part{
            .id = require_uint16_from_key(key, path_prefix),
            .path = require_string(item, "path", join_path(path_prefix, "path")),
            .frame_size = std::nullopt,
            .visible_size = require_vec2(item, "visible_size",
                                         join_path(path_prefix, "visible_size")),
            .visible_offset = require_vec2(item, "visible_offset",
                                           join_path(path_prefix, "visible_offset")),
            .draw_offset = optional_vec2_or_default(item, "draw_offset", SpriteVec2{0, 0},
                                                    join_path(path_prefix, "draw_offset")),
            .directions = {},
            .rows = {},
    };

    const toml::table& directions =
            require_subtable(item, "directions", join_path(path_prefix, "directions"));
    for (const auto& [direction_key, direction_value] : directions) {
        const std::string direction_name(direction_key.str());
        const std::string direction_path =
                join_path(join_path(path_prefix, "directions"), direction_name);
        const toml::table* direction_table = direction_value.as_table();
        if (!direction_table) {
            throw_invalid(direction_path, "se esperaba una tabla");
        }
        part.directions[row_index_for_key(direction_name)] =
                SpriteSheetDirection{.src = require_rect4(*direction_table, "src",
                                                         join_path(direction_path, "src"))};
    }

    validate_directions(part, path_prefix);
    return part;
}

CharacterPartDefinition parse_body_part(const toml::key& key, const toml::table& item,
                                        const std::string& path_prefix) {
    CharacterPartDefinition part{
            .id = require_uint16_from_key(key, path_prefix),
            .path = require_string(item, "path", join_path(path_prefix, "path")),
            .frame_size = require_vec2(item, "frame_size", join_path(path_prefix, "frame_size")),
            .visible_size = require_vec2(item, "visible_size",
                                         join_path(path_prefix, "visible_size")),
            .visible_offset = require_vec2(item, "visible_offset",
                                           join_path(path_prefix, "visible_offset")),
            .draw_offset = optional_vec2_or_default(item, "draw_offset", SpriteVec2{0, 0},
                                                    join_path(path_prefix, "draw_offset")),
            .directions = {},
            .rows = {},
    };

    const toml::table& rows = require_subtable(item, "rows", join_path(path_prefix, "rows"));
    for (const auto& [row_key, row_value] : rows) {
        const std::string row_name(row_key.str());
        const std::string row_path = join_path(join_path(path_prefix, "rows"), row_name);
        const toml::table* row_table = row_value.as_table();
        if (!row_table) {
            throw_invalid(row_path, "se esperaba una tabla");
        }
        part.rows[row_index_for_key(row_name)] = SpriteSheetAnimationRow{
                .y = require_int(*row_table, "y", join_path(row_path, "y")),
                .frames = require_int(*row_table, "frames", join_path(row_path, "frames")),
                .step_x = require_int(*row_table, "step_x", join_path(row_path, "step_x")),
        };
    }

    validate_rows(part, path_prefix);
    return part;
}

std::unordered_map<std::string, SkinPreset> parse_skins(const toml::table& root) {
    std::unordered_map<std::string, SkinPreset> skins_map;
    const toml::table& skins_table = require_subtable(root, "skins", "skins");
    for (const auto& [key, value] : skins_table) {
        const std::string skin_name(key.str());
        const toml::table* skin_table = value.as_table();
        if (!skin_table) {
            throw_invalid(join_path("skins", skin_name), "se esperaba una tabla");
        }

        skins_map.emplace(
                skin_name,
                SkinPreset{
                        .head_ids = require_uint16_array(
                                *skin_table, "head_ids",
                                join_path(join_path("skins", skin_name), "head_ids")),
                        .body_ids = require_uint16_array(
                                *skin_table, "body_ids",
                                join_path(join_path("skins", skin_name), "body_ids")),
                });
    }
    return skins_map;
}

std::pair<std::unordered_map<uint16_t, CharacterPartDefinition>,
          std::unordered_map<uint16_t, CharacterPartDefinition>>
parse_character_parts(const toml::table& root) {
    std::unordered_map<uint16_t, CharacterPartDefinition> heads_map;
    std::unordered_map<uint16_t, CharacterPartDefinition> bodies_map;
    const toml::table& character_parts =
            require_subtable(root, "character_parts", "character_parts");
    const toml::table& heads =
            require_subtable(character_parts, "heads", "character_parts.heads");
    const toml::table& bodies =
            require_subtable(character_parts, "bodies", "character_parts.bodies");

    for (const auto& [key, value] : heads) {
        const std::string path_prefix =
                join_path("character_parts.heads", std::string(key.str()));
        const toml::table* item = value.as_table();
        if (!item) {
            throw_invalid(path_prefix, "se esperaba una tabla");
        }
        const auto part = parse_head_part(key, *item, path_prefix);
        heads_map.emplace(part.id, part);
    }

    for (const auto& [key, value] : bodies) {
        const std::string path_prefix =
                join_path("character_parts.bodies", std::string(key.str()));
        const toml::table* item = value.as_table();
        if (!item) {
            throw_invalid(path_prefix, "se esperaba una tabla");
        }
        const auto part = parse_body_part(key, *item, path_prefix);
        bodies_map.emplace(part.id, part);
    }
    return {std::move(heads_map), std::move(bodies_map)};
}

std::unordered_map<std::string, StateOverride> parse_states(const toml::table& root) {
    std::unordered_map<std::string, StateOverride> states_map;
    if (const toml::table* states = root["states"].as_table()) {
        for (const auto& [key, value] : *states) {
            const std::string state_name(key.str());
            const std::string path_prefix = join_path("states", state_name);
            const toml::table* item = value.as_table();
            if (!item) {
                throw_invalid(path_prefix, "se esperaba una tabla");
            }

            StateOverride state;
            if (auto head_path = item->get_as<std::string>("head_path")) {
                state.head_path = head_path->get();
            }
            if (auto body_path = item->get_as<std::string>("body_path")) {
                state.body_path = body_path->get();
            }
            if (!state.head_path.has_value() && !state.body_path.has_value()) {
                throw_invalid(path_prefix, "debe definir head_path o body_path");
            }

            states_map.emplace(state_name, std::move(state));
        }
    }
    return states_map;
}

std::unordered_map<std::string, StaticSpriteDefinition> parse_creatures(const toml::table& root) {
    std::unordered_map<std::string, StaticSpriteDefinition> creatures_map;
    if (const toml::table* creatures = root["creatures"].as_table()) {
        for (const auto& [key, value] : *creatures) {
            const std::string creature_name(key.str());
            const std::string path_prefix = join_path("creatures", creature_name);
            const toml::table* item = value.as_table();
            if (!item) {
                throw_invalid(path_prefix, "se esperaba una tabla");
            }

            creatures_map.emplace(
                    creature_name,
                    StaticSpriteDefinition{
                            .part = require_string(*item, "part", join_path(path_prefix, "part")),
                            .path = require_string(*item, "path", join_path(path_prefix, "path")),
                            .src = require_rect4(*item, "src", join_path(path_prefix, "src")),
                            .size = require_vec2(*item, "size", join_path(path_prefix, "size")),
                            .offset = require_vec2(*item, "offset",
                                                   join_path(path_prefix, "offset")),
                    });
        }
    }
    return creatures_map;
}

std::unordered_map<std::string, NpcSpriteDefinition> parse_npcs(const toml::table& root) {
    std::unordered_map<std::string, NpcSpriteDefinition> npcs_map;
    if (const toml::table* npcs = root["npcs"].as_table()) {
        for (const auto& [key, value] : *npcs) {
            const std::string npc_name(key.str());
            const std::string path_prefix = join_path("npcs", npc_name);
            const toml::table* item = value.as_table();
            if (!item) {
                throw_invalid(path_prefix, "se esperaba una tabla");
            }

            NpcSpriteDefinition npc;
            if (auto head_path = item->get_as<std::string>("head_path")) {
                npc.head_path = head_path->get();
            }
            if (auto body_path = item->get_as<std::string>("body_path")) {
                npc.body_path = body_path->get();
            }
            npc.size = optional_vec2_or_default(*item, "size", SpriteVec2{17, 40},
                                                join_path(path_prefix, "size"));
            npc.src_head = require_rect4(*item, "src", join_path(path_prefix, "src")),
            npc.src = require_rect4(*item, "src", join_path(path_prefix, "src")),
            npc.offset = optional_vec2_or_default(*item, "offset", SpriteVec2{0, 0},
                                                  join_path(path_prefix, "offset"));
            if (!npc.head_path.has_value() && !npc.body_path.has_value()) {
                throw_invalid(path_prefix, "debe definir head_path o body_path");
            }

            npcs_map.emplace(npc_name, std::move(npc));
        }
    }
    return npcs_map;
}

void validate_catalog(const std::unordered_map<std::string, SkinPreset>& skins,
                      const std::unordered_map<uint16_t, CharacterPartDefinition>& heads,
                      const std::unordered_map<uint16_t, CharacterPartDefinition>& bodies) {
    for (const auto& [skin_name, preset] : skins) {
        validate_skin_parts_exist(skin_name, preset, heads, bodies);
    }
}

}  // namespace

SpriteCatalog SpriteCatalog::load_from_file(const std::string& path) {
    const toml::table root = toml::parse_file(path);
    SpriteCatalog catalog;

    catalog.skins_ = parse_skins(root);
    auto [heads, bodies] = parse_character_parts(root);
    catalog.heads_ = std::move(heads);
    catalog.bodies_ = std::move(bodies);
    catalog.states_ = parse_states(root);
    catalog.creatures_ = parse_creatures(root);
    catalog.npcs_ = parse_npcs(root);
    validate_catalog(catalog.skins_, catalog.heads_, catalog.bodies_);

    return catalog;
}

const SkinPreset& SpriteCatalog::skin_preset(const std::string& key) const {
    return skins_.at(key);
}

const CharacterPartDefinition& SpriteCatalog::head(uint16_t id) const {
    return heads_.at(id);
}

const CharacterPartDefinition& SpriteCatalog::body(uint16_t id) const {
    return bodies_.at(id);
}

const StateOverride* SpriteCatalog::state_override(const std::string& key) const {
    const auto it = states_.find(key);
    return it == states_.end() ? nullptr : &it->second;
}

const StaticSpriteDefinition& SpriteCatalog::creature(const std::string& key) const {
    return creatures_.at(key);
}

const NpcSpriteDefinition& SpriteCatalog::npc(const std::string& key) const {
    return npcs_.at(key);
}

bool SpriteCatalog::has_skin_preset(const std::string& key) const {
    return skins_.contains(key);
}

bool SpriteCatalog::has_head(uint16_t id) const {
    return heads_.contains(id);
}

bool SpriteCatalog::has_body(uint16_t id) const {
    return bodies_.contains(id);
}

bool SpriteCatalog::has_state_override(const std::string& key) const {
    return states_.contains(key);
}

bool SpriteCatalog::has_creature(const std::string& key) const {
    return creatures_.contains(key);
}

bool SpriteCatalog::has_npc(const std::string& key) const {
    return npcs_.contains(key);
}
