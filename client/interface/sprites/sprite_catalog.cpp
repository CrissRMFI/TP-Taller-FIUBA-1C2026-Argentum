
#include "sprite_catalog.h"
#include <string>
#include <toml++/toml.hpp>
#include <unordered_map>

SpriteCatalogParser SpriteCatalog::get_parser() const {
    return this->parser;
}

SpriteCatalog SpriteCatalog::load_from_file(const std::string& path) {
    const toml::table root = toml::parse_file(path);
    SpriteCatalog catalog;
    SpriteCatalogParser parser_ = catalog.get_parser();

    catalog.skins = parser_.parse_skins(root);
    auto [heads, bodies] = catalog.parser.parse_character_parts(root);
    catalog.heads = std::move(heads);
    catalog.bodies = std::move(bodies);
    catalog.states = parser_.parse_states(root);
    catalog.creatures = parser_.parse_creatures(root);
    for (const auto& [key, creature] : catalog.creatures) {
        catalog.creatures_by_id.emplace(creature.id, key);
    }
    catalog.npcs = parser_.parse_npcs(root);
    parser_.validate_catalog(catalog.skins, catalog.heads, catalog.bodies);

    return catalog;
}

const SkinPreset& SpriteCatalog::skin_preset(const std::string& key) const {
    return skins.at(key);
}

const CharacterPartDefinition& SpriteCatalog::head(uint16_t id) const {
    return heads.at(id);
}

const CharacterPartDefinition& SpriteCatalog::body(uint16_t id) const {
    return bodies.at(id);
}

const StateOverride* SpriteCatalog::state_override(const std::string& key) const {
    const auto it = states.find(key);
    return it == states.end() ? nullptr : &it->second;
}

const StaticSpriteDefinition& SpriteCatalog::creature(const std::string& key) const {
    return creatures.at(key);
}

const StaticSpriteDefinition& SpriteCatalog::creature(const uint16_t id) const {
    return creatures.at(creatures_by_id.at(id));
}

const NpcSpriteDefinition& SpriteCatalog::npc(const std::string& key) const {
    return npcs.at(key);
}

bool SpriteCatalog::has_skin_preset(const std::string& key) const {
    return skins.contains(key);
}

bool SpriteCatalog::has_head(uint16_t id) const {
    return heads.contains(id);
}

bool SpriteCatalog::has_body(uint16_t id) const {
    return bodies.contains(id);
}

bool SpriteCatalog::has_state_override(const std::string& key) const {
    return states.contains(key);
}

bool SpriteCatalog::has_creature(const std::string& key) const {
    return creatures.contains(key);
}

bool SpriteCatalog::has_creature(const uint16_t id) const {
    return creatures_by_id.contains(id);
}

bool SpriteCatalog::has_npc(const std::string& key) const {
    return npcs.contains(key);
}
