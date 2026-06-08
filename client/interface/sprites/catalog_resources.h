//
// Created by victoria zubieta on 07/06/2026.
//

#ifndef TALLER_TP_CATALOG_RESOURCES_H
#define TALLER_TP_CATALOG_RESOURCES_H
#include <cstdint>
#include <vector>

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
    int column;
    SpriteRect src;
};

struct SpriteSheetAnimationRow {
    int y;
    int frames;
    int step_x;
    int row;
};

struct CharacterPartDefinition {
    uint16_t id;
    std::string path;
    SpriteRect scr_head;
    SpriteRect scr_body;
    std::optional<SpriteVec2> frame_size;
    SpriteVec2 draw_offset;
    std::array<std::optional<SpriteSheetDirection>, 4> directions;
    std::array<std::optional<SpriteSheetAnimationRow>, 4> rows;
};

struct StateOverride {
    std::optional<std::string> head_path;
    std::optional<std::string> body_path;
    std::optional<SpriteRect> body_src;
};

struct StaticSpriteDefinition {
    uint16_t id;
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
#endif  // TALLER_TP_CATALOG_RESOURCES_H
