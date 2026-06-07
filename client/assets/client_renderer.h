//
// Created by victoria zubieta on 29/05/2026.
//

#ifndef TALLER_TP_CLIENT_RENDERER_H
#define TALLER_TP_CLIENT_RENDERER_H
#include <memory>
#include <unordered_set>
#include <vector>

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
#include "estado_chat_render.h"
#include "estado_panel_render.h"
#include "npc_renderer.h"
#include "npc_sprite_resolver.h"
#include "object_animation.h"
#include "sprite_manager.h"
#include "text_renderer.h"
#include "../config/catalogo_items.h"
#include "../../common/game/mapa/mapa.h"

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
    std::unique_ptr<TextRenderer> text_renderer;
    std::unique_ptr<SDL2pp::Texture> chat_background_texture;
    ConfigChatRender chat_config;
    ConfigPanelRender panel_config;
    const CatalogoItems* catalogo = nullptr;
    std::unordered_set<uint16_t> iconos_fallidos;
    // FX de hechizo en curso: animacion transitoria sobre el objetivo.
    struct FxActivo {
        uint16_t spellId;
        uint16_t targetId;
        uint32_t startTick;
    };
    std::vector<FxActivo> fx_activos;
    int last_animation_row = -1;
    int window_width = 0;
    int window_height = 0;
    Mapa mapa;
    SDL_Color elegircolor(uint8_t tipo, uint8_t estado) const;
    Mapa cargarMapa() const;
    void dibujar_chat(const EstadoChatRender& chat);
    void dibujar_panel(const EstadoPanelRender& panel);
    void dibujar_banco(const EstadoBancoRender& banco);
    void dibujar_meditacion(int entity_x, int entity_y, int cell_width, int cell_height,
                            uint32_t tick);
    SDL2pp::Texture* icono_item(uint16_t id);
    int ancho_juego() const;
    std::vector<SDL2pp::Rect> banco_boveda;
    std::vector<SDL2pp::Rect> banco_inv;
    SDL2pp::Rect rect_dep{0, 0, 0, 0};
    SDL2pp::Rect rect_ret{0, 0, 0, 0};
    SDL2pp::Rect rect_dep_oro{0, 0, 0, 0};
    SDL2pp::Rect rect_ret_oro{0, 0, 0, 0};
    SDL2pp::Rect rect_caja_monto{0, 0, 0, 0};
    SDL2pp::Rect rect_cerrar_banco{0, 0, 0, 0};
    std::vector<SDL2pp::Rect> slots_inventario;
    std::vector<SDL2pp::Rect> slots_stock;
    std::vector<SDL2pp::Rect> slots_hechizos;       // filas de la pestaña HECHIZOS (lanzar)
    std::vector<uint16_t> ids_hechizos_dibujados;   // id de hechizo por fila (pestaña)
    std::vector<SDL2pp::Rect> slots_hechizos_venta; // filas de hechizos en venta del sacerdote
    std::vector<uint16_t> ids_hechizos_venta;       // id por fila de venta
    SDL2pp::Rect rect_boton_vender{0, 0, 0, 0};
    SDL2pp::Rect rect_boton_equipar{0, 0, 0, 0};
    SDL2pp::Rect rect_boton_usar{0, 0, 0, 0};
    SDL2pp::Rect rect_boton_curar{0, 0, 0, 0};
    SDL2pp::Rect rect_tab_inv{0, 0, 0, 0};   // pestaña INVENTARIO del marco
    SDL2pp::Rect rect_tab_hech{0, 0, 0, 0};  // pestaña HECHIZOS del marco
    int slot_en(const std::vector<SDL2pp::Rect>& slots, int x, int y) const;

public:
    ObjectRenderer();
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen,
              bool vsync, int loop_fps, const ConfigChatRender& chat_config,
              const ConfigPanelRender& panel_config, const CatalogoItems* catalogo);
    void update_animation(int it, const ObjectGameWorld& state_object, const ObjectAnimation& animation);
    void render(const ObjectGameWorld& state_object, const ObjectAnimation& animation,
                const EstadoChatRender& chat, const EstadoPanelRender& panel,
                const EstadoBancoRender& banco);
    int slotInventarioClickeado(int x, int y) const;
    int slotStockClickeado(int x, int y) const;
    bool clickEnBotonVender(int x, int y) const;
    bool clickEnBotonEquipar(int x, int y) const;
    bool clickEnBotonUsar(int x, int y) const;
    bool clickEnBotonCurar(int x, int y) const;
    // Devuelve el id del hechizo clickeado en la lista del panel, o 0 si ninguno.
    uint16_t hechizoClickeado(int x, int y) const;       // pestaña HECHIZOS (lanzar)
    uint16_t hechizoVentaClickeado(int x, int y) const;  // lista del sacerdote (comprar)
    bool clickTabInventario(int x, int y) const;
    bool clickTabHechizos(int x, int y) const;
    bool esSacerdote(uint16_t id) const;
    // Inicia la animacion de FX de un hechizo sobre un objetivo (se dibuja unos frames).
    void iniciarFx(uint16_t spellId, uint16_t targetId);
    // Banco: hit-test (devuelven indice de slot o -1; los botones true/false).
    int bancoBovedaClickeada(int x, int y) const;
    int bancoInvClickeado(int x, int y) const;
    bool clickBancoDepositar(int x, int y) const;
    bool clickBancoRetirar(int x, int y) const;
    bool clickBancoDepositarOro(int x, int y) const;
    bool clickBancoRetirarOro(int x, int y) const;
    bool clickBancoCajaMonto(int x, int y) const;
    bool clickBancoCerrar(int x, int y) const;
    void otroUsuario(SDL2pp::Texture texture, uint8_t tipo, uint8_t estado);
};

#endif
