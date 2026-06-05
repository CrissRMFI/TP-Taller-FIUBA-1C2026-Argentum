#ifndef TALLER_TP_SPRITE_CATALOG_H
#define TALLER_TP_SPRITE_CATALOG_H

#include <cstdint>
#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// clase encargada de leer el archivo de las imagenes
struct SpriteRect {
    int x;
    int y;
    int width;
    int height;
};

struct SpriteVec2 {
    int x;
    int y;
};

struct SkinPreset {
    std::vector<uint16_t> head_ids;
    std::vector<uint16_t> body_ids;
};

struct SpriteSheetDirection {
    SpriteRect src;
};

struct SpriteSheetAnimationRow {
    int y;
    int frames;
    int step_x;
};

struct CharacterPartDefinition {
    uint16_t id;
    std::string path;
    std::optional<SpriteVec2> frame_size;
    SpriteVec2 visible_size;
    SpriteVec2 visible_offset;
    SpriteVec2 draw_offset;
    std::array<std::optional<SpriteSheetDirection>, 4> directions;
    std::array<std::optional<SpriteSheetAnimationRow>, 4> rows;
};

struct StateOverride {
    std::optional<std::string> head_path;
    std::optional<std::string> body_path;
};

struct StaticSpriteDefinition {
    std::string part;
    std::string path;
    SpriteRect src;
    SpriteVec2 size;
    SpriteVec2 offset;

};

struct NpcSpriteDefinition {
    std::optional<std::string> head_path;
    std::optional<std::string> body_path;
    SpriteRect src_head;
    SpriteRect src;
    SpriteVec2 size;
    SpriteVec2 offset;
};

class SpriteCatalog {
private:
    std::unordered_map<std::string, SkinPreset> skins_;
    std::unordered_map<uint16_t, CharacterPartDefinition> heads_;
    std::unordered_map<uint16_t, CharacterPartDefinition> bodies_;
    std::unordered_map<std::string, StateOverride> states_;
    std::unordered_map<std::string, StaticSpriteDefinition> creatures_;
    std::unordered_map<std::string, NpcSpriteDefinition> npcs_;

public:
    static SpriteCatalog load_from_file(const std::string& path);

    const SkinPreset& skin_preset(const std::string& key) const;
    const CharacterPartDefinition& head(uint16_t id) const;
    const CharacterPartDefinition& body(uint16_t id) const;
    const StateOverride* state_override(const std::string& key) const;
    const StaticSpriteDefinition& creature(const std::string& key) const;
    const NpcSpriteDefinition& npc(const std::string& key) const;

    bool has_skin_preset(const std::string& key) const;
    bool has_head(uint16_t id) const;
    bool has_body(uint16_t id) const;
    bool has_state_override(const std::string& key) const;
    bool has_creature(const std::string& key) const;
    bool has_npc(const std::string& key) const;
};

#endif  // TALLER_TP_SPRITE_CATALOG_H
