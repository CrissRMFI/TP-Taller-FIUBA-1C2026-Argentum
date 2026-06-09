#ifndef TALLER_TP_SPRITE_CATALOG_H
#define TALLER_TP_SPRITE_CATALOG_H

#include <cstdint>
#include <string>
#include <unordered_map>


// clase encargada de leer el archivo de las imagenes

#include "sprite_catalog_parser.h"

class SpriteCatalog {
private:
    SpriteCatalogParser parser;
    std::unordered_map<std::string, SkinPreset> skins;
    std::unordered_map<uint16_t, CharacterPartDefinition> heads;
    std::unordered_map<uint16_t, CharacterPartDefinition> bodies;
    std::unordered_map<std::string, StateOverride> states;
    std::unordered_map<std::string, StaticSpriteDefinition> creatures;
    std::unordered_map<uint16_t, std::string> creatures_by_id;
    std::unordered_map<std::string, NpcSpriteDefinition> npcs;

public:
    SpriteCatalogParser get_parser() const;
    static SpriteCatalog load_from_file(const std::string& path);

    const SkinPreset& skin_preset(const std::string& key) const;
    const CharacterPartDefinition& head(uint16_t id) const;
    const CharacterPartDefinition& body(uint16_t id) const;
    const StateOverride* state_override(const std::string& key) const;
    const StaticSpriteDefinition& creature(const std::string& key) const;
    const StaticSpriteDefinition& creature(uint16_t id) const;
    const NpcSpriteDefinition& npc(const std::string& key) const;

    bool has_skin_preset(const std::string& key) const;
    bool has_head(uint16_t id) const;
    bool has_body(uint16_t id) const;
    bool has_state_override(const std::string& key) const;
    bool has_creature(const std::string& key) const;
    bool has_creature(uint16_t id) const;
    bool has_npc(const std::string& key) const;
};

#endif  // TALLER_TP_SPRITE_CATALOG_H
