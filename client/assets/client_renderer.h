//
// Created by victoria zubieta on 29/05/2026.
//

#ifndef TALLER_TP_CLIENT_RENDERER_H
#define TALLER_TP_CLIENT_RENDERER_H
#include <memory>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Texture.hh"
#include "SDL2pp/Window.hh"
#include "character_renderer.h"
#include "character_sprite_resolver.h"
#include "client_game_world.h"
#include "criatura_renderer.h"
#include "criatura_sprite_resolver.h"
#include "npc_renderer.h"
#include "npc_sprite_resolver.h"
#include "object_animation.h"
#include "sprite_manager.h"

// se encarga de encargar las texturas y de actualizar su estado de acuerdo al movimiento
class ObjectRenderer {
private:
    std::unique_ptr<SDL2pp::SDL> sdl;
    std::unique_ptr<SDL2pp::SDLImage> image_context;
    std::unique_ptr<SDL2pp::Window> window;
    std::unique_ptr<SDL2pp::Renderer> renderer;
    std::unique_ptr<SDL2pp::Texture> background_texture;
    std::unique_ptr<SpriteManager> sprite_manager;
    std::unique_ptr<CharacterSpriteResolver> resolver_sprite;
    std::unique_ptr<CharacterRenderer> character_renderer;
    std::unique_ptr<CreatureSpriteResolver> creature_sprite_resolver;
    std::unique_ptr<CriaturaRenderer> criatura_renderer;
    std::unique_ptr<NpcSpriteResolver> npc_sprite_resolver;
    std::unique_ptr<SpriteCatalog> catalog;
    std::unique_ptr<TextureCache> cache_texture;
    std::unique_ptr<NPCRenderer> npc_renderer;
    int last_animation_row = -1;
    int window_width = 0;
    int window_height = 0;

    SDL_Color elegircolor(uint8_t tipo, uint8_t estado) const;

public:
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void update_animation(uint32_t current_tick,
                          const ObjectGameWorld& state_object,
                          const ObjectAnimation& animation);
    void render(const ObjectGameWorld& state_object, const ObjectAnimation& animation);
    void otroUsuario(SDL2pp::Texture texture, uint8_t tipo, uint8_t estado);
};

#endif  // TALLER_TP_CLIENT_RENDERER_H
