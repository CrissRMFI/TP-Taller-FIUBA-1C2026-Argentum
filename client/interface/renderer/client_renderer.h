#ifndef TALLER_TP_CLIENT_RENDERER_H
#define TALLER_TP_CLIENT_RENDERER_H
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

#include "../../../common/game/mapa/mapa.h"
#include "../../../common/game/mapa/portal.h"
#include "../../../common/persistencia/lector_mapa.h"
#include "../../camara/player_camera.h"
#include "../../config/catalogo_items.h"
#include "../client_game_world.h"
#include "../object_animation.h"
#include "../sprite_manager.h"
#include "../sprites_resolver/character_sprite_resolver.h"
#include "../sprites_resolver/criatura_sprite_resolver.h"
#include "../sprites_resolver/npc_sprite_resolver.h"
#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/SDLImage.hh"
#include "SDL2pp/Texture.hh"
#include "SDL2pp/Window.hh"
#include "character_renderer.h"
#include "criatura_renderer.h"
#include "estado_chat_render.h"
#include "estado_panel_render.h"
#include "npc_renderer.h"
#include "text_renderer.h"

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
    // Proyectil en vuelo: viaja del origen al destino y desaparece.
    struct ProyectilActivo {
        uint16_t origen;
        uint16_t destino;
        uint32_t startTick;
    };
    std::vector<ProyectilActivo> proyectiles_;
    uint16_t objetivo_resaltado = 0;
    uint16_t hover_resaltado = 0;
    int last_animation_row = -1;
    double vis_player_x = 0.0;
    double vis_player_y = 0.0;
    bool vis_init = false;
    uint32_t vis_last_tick = 0;
    uint32_t walk_tile_ms = 130;
    int window_width = 0;
    int window_height = 0;
    std::map<uint16_t, Mapa> mapas;
    std::vector<Portal> portales;
    uint16_t mapaActual = 0;
    uint16_t mapaPrincipalId = 0;
    PlayerCamera camera;
    SDL_Color elegircolor(uint8_t tipo, uint8_t estado) const;
    WorldCargado cargarMundo() const;
    const Mapa& mapaVigente() const;
    void dibujar_chat(const EstadoChatRender& chat);
    void dibujar_panel(const EstadoPanelRender& panel);
    void dibujar_banco(const EstadoBancoRender& banco);
    void dibujar_tienda(const EstadoTiendaRender& tienda);
    void dibujar_meditacion(int entity_x, int entity_y, int cell_width, int cell_height,
                            uint32_t tick);

    void dibujar_resurreccion(int entity_x, int entity_y, int cell_width, int cell_height,
                              uint32_t tick);

    void dibujar_barra_resurreccion(int entity_x, int entity_y, int cell_width, int cell_height,
                                    float fraccion);
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
    // Modal de tienda (comerciante / sacerdote).
    std::vector<SDL2pp::Rect> tienda_oferta;
    std::vector<SDL2pp::Rect> tienda_inv;
    SDL2pp::Rect rect_tienda_comprar{0, 0, 0, 0};
    SDL2pp::Rect rect_tienda_vender{0, 0, 0, 0};
    SDL2pp::Rect rect_tienda_curar{0, 0, 0, 0};  // solo sacerdote
    SDL2pp::Rect rect_tienda_cerrar{0, 0, 0, 0};
    std::vector<SDL2pp::Rect> slots_equipo;
    std::vector<SDL2pp::Rect> slots_inventario;
    std::vector<SDL2pp::Rect> slots_stock;
    std::vector<SDL2pp::Rect> slots_hechizos;        // filas de la pestaña HECHIZOS (lanzar)
    std::vector<uint16_t> ids_hechizos_dibujados;    // id de hechizo por fila (pestaña)
    std::vector<SDL2pp::Rect> slots_hechizos_venta;  // filas de hechizos en venta del sacerdote
    std::vector<uint16_t> ids_hechizos_venta;        // id por fila de venta
    SDL2pp::Rect rect_boton_vender{0, 0, 0, 0};
    SDL2pp::Rect rect_tab_inv{0, 0, 0, 0};   // pestaña INVENTARIO del marco
    SDL2pp::Rect rect_tab_hech{0, 0, 0, 0};  // pestaña HECHIZOS del marco
    int slot_en(const std::vector<SDL2pp::Rect>& slots, int x, int y) const;

public:
    ObjectRenderer();
    int anchoMapa() const {
        return mapaVigente().getAncho();
    }
    int altoMapa() const {
        return mapaVigente().getAlto();
    }
    int bordeIzquierdoPanel() const {
        return ancho_juego();
    }
    void setMapaActual(uint16_t id) {
        mapaActual = id;
    }
    uint16_t getMapaPrincipal() const {
        return mapaPrincipalId;
    }  // exterior; otros = mazmorra
    int camOffsetX() const {
        return camera.get_offset_x();
    }
    int camOffsetY() const {
        return chat_config.panelAlto + camera.get_offset_y();
    }
    int camTileW() const {
        return camera.tile_width();
    }
    int camTileH() const {
        return camera.tile_height();
    }
    void zoomIn() {
        camera.zoom_in();
    }
    void zoomOut() {
        camera.zoom_out();
    }
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen,
              bool vsync, int loop_fps, const ConfigChatRender& chat_config,
              const ConfigPanelRender& panel_config, const CatalogoItems* catalogo,
              const ConfigCamara& camara_config, uint32_t walk_tile_ms);
    void update_animation(int it, const ObjectGameWorld& state_object,
                          const ObjectAnimation& animation);
    void render(const ObjectGameWorld& state_object, const ObjectAnimation& animation,
                const EstadoChatRender& chat, const EstadoPanelRender& panel,
                const EstadoBancoRender& banco, const EstadoTiendaRender& tienda);
    int slotEquipoClickeado(int x, int y) const;
    int slotInventarioClickeado(int x, int y) const;
    int slotStockClickeado(int x, int y) const;
    bool clickEnBotonVender(int x, int y) const;
    // Devuelve el id del hechizo clickeado en la lista del panel, o 0 si ninguno.
    uint16_t hechizoClickeado(int x, int y) const;       // pestaña HECHIZOS (lanzar)
    uint16_t hechizoVentaClickeado(int x, int y) const;  // lista del sacerdote (comprar)
    bool clickTabInventario(int x, int y) const;
    bool clickTabHechizos(int x, int y) const;
    bool esSacerdote(uint16_t id) const;
    bool esBanquero(uint16_t id) const;
    bool esComerciante(uint16_t id) const;
    void iniciarFx(uint16_t spellId, uint16_t targetId);
    void iniciarProyectil(uint16_t origen, uint16_t destino);
    void resaltarObjetivo(uint16_t id);
    void resaltarHover(uint16_t id);
    int bancoBovedaClickeada(int x, int y) const;
    int bancoInvClickeado(int x, int y) const;
    bool clickBancoDepositar(int x, int y) const;
    bool clickBancoRetirar(int x, int y) const;
    bool clickBancoDepositarOro(int x, int y) const;
    bool clickBancoRetirarOro(int x, int y) const;
    bool clickBancoCajaMonto(int x, int y) const;
    bool clickBancoCerrar(int x, int y) const;
    // Tienda (comerciante/sacerdote): hit-tests.
    int tiendaOfertaClickeada(int x, int y) const;
    int tiendaInvClickeado(int x, int y) const;
    bool clickTiendaComprar(int x, int y) const;
    bool clickTiendaVender(int x, int y) const;
    bool clickTiendaCurar(int x, int y) const;
    bool clickTiendaCerrar(int x, int y) const;
    void otroUsuario(SDL2pp::Texture texture, uint8_t tipo, uint8_t estado);
};

#endif
