#ifndef TALLER_TP_TEXTURE_CACHE_H
#define TALLER_TP_TEXTURE_CACHE_H

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/Texture.hh"

class TextureCache {
private:
    SDL2pp::Renderer& renderer_;
    std::string resources_root_;
    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> textures_;

    std::string resolve_path(const std::string& relative_path) const;

public:
    TextureCache(SDL2pp::Renderer& renderer, std::string resources_root);

    SDL2pp::Texture& get_or_load(const std::string& relative_path);
    bool has(const std::string& relative_path) const;
    void clear();
};

#endif  // TALLER_TP_TEXTURE_CACHE_H
