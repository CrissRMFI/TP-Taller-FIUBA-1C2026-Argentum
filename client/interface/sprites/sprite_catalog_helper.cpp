//
// Created by victoria zubieta on 06/06/2026.
//

#include "sprite_catalog_helper.h"
#include <string>
#include <toml++/toml.hpp>
#include <unordered_map>


std::string SpriteCatalogHelper::join_path(const std::string& a, const std::string& b) {
    if (a.empty()) {
        return b;
    }
    return a + "." + b;
}

int SpriteCatalogHelper::row_index_for_key(const std::string& key) {
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

void SpriteCatalogHelper::throw_missing(const std::string& path) {
    throw std::runtime_error("Falta config TOML obligatoria: " + path);
}

void SpriteCatalogHelper::throw_invalid(const std::string& path, const std::string& detail) {
    throw std::runtime_error("Config TOML invalida en " + path + ": " + detail);
}

const toml::table& SpriteCatalogHelper::require_subtable(const toml::table& table, const std::string& key,
                                    const std::string& path) {
    const toml::table* child = table[key].as_table();
    if (!child || !child->is_table()) {
        throw_missing("Se esperaba tabla en: " + path);
    }
    return *child->as_table();
}

std::string SpriteCatalogHelper::require_string(const toml::table& table, const std::string& key,
                           const std::string& path) {
    auto value = table[key].value<std::string>();
    if (!value.has_value() || value->empty()) {
        throw_missing(path);
    }
    return *value;
}

int SpriteCatalogHelper::require_int(const toml::table& table, const std::string& key, const std::string& path) {
    auto value = table[key].value<int64_t>();
    if (!value.has_value()) {
        throw_missing(path);
    }
    return static_cast<int>(*value);
}

int SpriteCatalogHelper::require_int_any_key(const toml::table& table,
                        const std::initializer_list<std::string>& keys,
                        const std::string& path) {
    for (const std::string& key : keys) {
        auto value = table[key].value<int64_t>();
        if (value.has_value()) {
            return static_cast<int>(*value);
        }
    }
    throw_missing(path);
    return 0;
}

uint16_t SpriteCatalogHelper::require_uint16_from_key(const toml::key& key, const std::string& path) {
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

std::vector<uint16_t> SpriteCatalogHelper::require_uint16_array(const toml::table& table, const std::string& key,
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

SpriteVec2 SpriteCatalogHelper::require_vec2(const toml::table& table, const std::string& key,
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

SpriteVec2 SpriteCatalogHelper::optional_vec2_or_default(const toml::table& table, const std::string& key,
                                    const SpriteVec2 default_value,
                                    const std::string& path) {
    if (!table.contains(key)) {
        return default_value;
    }
    return require_vec2(table, key, path);
}

SpriteRect SpriteCatalogHelper::require_rect4(const toml::table& table, const std::string& key,
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

std::optional<SpriteRect> SpriteCatalogHelper::optional_rect4(const toml::table& table, const std::string& key,
                                         const std::string& path) {
    if (!table.contains(key)) {
        return std::nullopt;
    }
    return require_rect4(table, key, path);
}

void SpriteCatalogHelper::validate_skin_parts_exist(const std::string& skin_name, const SkinPreset& preset,
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

void SpriteCatalogHelper::validate_rows(const CharacterPartDefinition& part, const std::string& path) {
    for (int index = 0; index < 4; ++index) {
        if (!part.rows[index].has_value()) {
            throw_invalid(path, "faltan filas de animacion");
        }
        if (part.rows[index]->frames <= 0 || part.rows[index]->step_x <= 0) {
            throw_invalid(path, "frames y step_x deben ser positivos");
        }
    }
}

void SpriteCatalogHelper::validate_directions(const CharacterPartDefinition& part, const std::string& path) {
    for (int index = 0; index < 4; ++index) {
        if (!part.directions[index].has_value()) {
            throw_invalid(path, "faltan direcciones");
        }
    }
}


