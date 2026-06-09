#include "texture_cache.h"

#include <filesystem>
#include <utility>

#include "SDL.h"
#include "SDL2pp/Surface.hh"

TextureCache::TextureCache(SDL2pp::Renderer& renderer, std::string resources_root):
    renderer_(renderer), resources_root_(std::move(resources_root)) {}

std::string TextureCache::resolve_path(const std::string& relative_path) const {
    const std::filesystem::path base(resources_root_);
    return (base / relative_path).string();
}

SDL2pp::Texture& TextureCache::get_or_load(const std::string& relative_path) {
    const auto it = textures_.find(relative_path);
    if (it != textures_.end()) {
        return *it->second;
    }

    SDL2pp::Surface surface(resolve_path(relative_path));
    SDL_SetColorKey(surface.Get(), SDL_TRUE,
                    SDL_MapRGB(surface.Get()->format, 0, 0, 0));
    auto texture = std::make_unique<SDL2pp::Texture>(renderer_, surface);
    SDL2pp::Texture& texture_ref = *texture;
    textures_.emplace(relative_path, std::move(texture));
    return texture_ref;
}

bool TextureCache::has(const std::string& relative_path) const {
    return textures_.contains(relative_path);
}

void TextureCache::clear() {
    textures_.clear();
}
