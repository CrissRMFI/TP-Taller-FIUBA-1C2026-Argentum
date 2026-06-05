//
// Created by victoria zubieta on 02/06/2026.
//

#include "npc_renderer.h"

#include "common/game/modelo/posicion.h"
#include "common/game/npc/npc.h"
namespace {
    uint16_t cuerpo_por_tipo_npc(const TipoNpc tipo) {
        switch (tipo) {
            case TipoNpc::Comerciante:
                return 4071;
            case TipoNpc::Sacerdote:
                return 1243;
            case TipoNpc::Banquero:
                return 4057;
        }

        return 0;
    }

    EntidadRenderizable entidad_desde_npc(const Npc& npc) {
        const Posicion posicion = npc.getPosicion();
        return EntidadRenderizable{
            .x = posicion.x,
            .y = posicion.y,
            .tipo = 2,
            .estado = 0,
            .cabeza = 0,
            .cuerpo = cuerpo_por_tipo_npc(npc.getTipo()),
        };
    }
    SDL2pp::Rect to_sdl_rect(const SpriteRect& rect) {
        return SDL2pp::Rect(rect.x, rect.y, rect.width, rect.height);
    }
} //namespace

NPCRenderer::NPCRenderer(NpcSpriteResolver& resolver): resolver_(resolver) {}

void NPCRenderer::render(SDL2pp::Renderer& renderer, const Npc& npc,
                         const int entity_x, const int entity_y, const int cell_width,
                         const int cell_height, const int /*animation_row*/,
                         const int /*frame_index*/) const {
    const auto entity = entidad_desde_npc(npc);
    const auto resolved = resolver_.resolve(entity);
    const int anchor_x = entity_x + cell_width / 2;
    const int anchor_y = entity_y + cell_height;
    const int body_width = std::max(1, static_cast<int>(resolved.size.x ));
    const int body_height = std::max(1, static_cast<int>(resolved.size.y));
    const int body_x = anchor_x - body_width / 2 + resolved.offset.x;
    const int body_y = anchor_y - body_height + resolved.offset.y;

    if (resolved.body.has_value()) {
        renderer.Copy(*resolved.body->texture, to_sdl_rect(resolved.src),
                      SDL2pp::Rect(body_x, body_y, body_width, body_height));
    }

    // if (resolved.body.has_value()) {
    //     renderer.Copy(*resolved.body->texture, SDL2pp::NullOpt,
    //                   SDL2pp::Rect(body_x, body_y, body_width, body_height));
    // }

    if (resolved.head.has_value()) {
        renderer.Copy(*resolved.head->texture, to_sdl_rect(resolved.src_head),
                      SDL2pp::Rect(body_x + 5, body_y + 1, body_width,body_height));
    }
}
