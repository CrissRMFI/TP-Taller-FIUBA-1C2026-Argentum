//
// Created by victoria zubieta on 02/06/2026.
//

#ifndef TALLER_TP_NPC_RENDERER_H
#define TALLER_TP_NPC_RENDERER_H

#include "SDL2pp/Renderer.hh"
#include "client/entidad_renderizable.h"
#include "common/game/npc/npc.h"
#include "client/interface/sprites_resolver/npc_sprite_resolver.h"

class NPCRenderer {
private:
    NpcSpriteResolver& resolver_;
    SDL2pp::Rect to_sdl_rect(const SpriteRect& rect) const;

public:

    explicit NPCRenderer(NpcSpriteResolver& resolver);

    void render(SDL2pp::Renderer& renderer,const Npc& npc, int entity_x,
                int entity_y, int cell_width, int cell_height, int animation_row,
                int frame_index) const;
};

#endif  // TALLER_TP_NPC_RENDERER_H
