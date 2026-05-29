//
// Created by victoria zubieta on 29/05/2026.
//

#ifndef TALLER_TP_CLIENT_RENDERER_H
#define TALLER_TP_CLIENT_RENDERER_H
#include <memory>

#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Texture.hh"
#include "SDL2pp/Window.hh"
#include "client_game_world.h"
#include "object_animation.h"
#include "sprite_manager.h"

// se encarga de encargar las texturas y de actualizar su estado de acuerdo al movimiento
class ObjectRenderer {
private:
    std::unique_ptr<SDL2pp::SDL> sdl;
    std::unique_ptr<SDL2pp::Window> window;
    std::unique_ptr<SDL2pp::Renderer> renderer;
    std::unique_ptr<SDL2pp::Texture> texture;
    std::unique_ptr<SpriteManager> sprite_manager;
    int last_animation_row = -1;

public:
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void update_animation(uint32_t current_tick,
                          const ObjectGameWorld& state_object,
                          const ObjectAnimation& animation);
    void render(const ObjectGameWorld& state_object, const ObjectAnimation& animation);
};


#endif  // TALLER_TP_CLIENT_RENDERER_H
