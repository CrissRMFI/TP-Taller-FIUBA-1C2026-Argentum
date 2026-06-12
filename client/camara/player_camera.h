#ifndef TALLER_TP_PLAYER_CAMERA_H
#define TALLER_TP_PLAYER_CAMERA_H

#include <cstdint>

// Parametros de zoom de la camara, cargados desde client_config.toml ([camara]).
struct ConfigCamara {
    int tiles_visibles_x = 35;
    int tiles_visibles_y = 30;
    int zoom_min_tiles_x = 12;
    int zoom_min_tiles_y = 10;
    int zoom_max_tiles_x = 60;
    int zoom_max_tiles_y = 50;
    int zoom_paso_tiles = 2;
};

class PlayerCamera {
private:
    int viewport_width = 0;
    int viewport_height = 0;
    int map_width_tiles = 0;
    int map_height_tiles = 0;
    int cell_width = 1;
    int cell_height = 1;
    int offset_x = 0;
    int offset_y = 0;
    int map_pixel_height = 0;
    int map_pixel_width = 0;
    // Zoom actual (tiles que entran en pantalla) y limites. Los valores reales se
    // cargan en aplicar_config() desde el TOML (defaults unicos en ConfigCamara);
    // estos son solo placeholders seguros (no-cero) por si se usa antes de configurar.
    int visible_tiles_x = 1;
    int visible_tiles_y = 1;
    int min_visible_tiles_x = 1;
    int min_visible_tiles_y = 1;
    int max_visible_tiles_x = 1;
    int max_visible_tiles_y = 1;
    int zoom_step_tiles = 1;

    void recalculate_scale();
    // Offset deseado (centrado en el tile y clampeado al mapa).
    void target_offset_for_tile(int tile_x, int tile_y, int& out_x, int& out_y) const;

public:
    void aplicar_config(const ConfigCamara& cfg);
    void configure(int view_width, int view_height, int map_width, int map_height);
    void center_on_tile(int tile_x, int tile_y);
    void center_on_point(double tile_x, double tile_y);
    int screen_x_for_tile(int tile_x) const;
    int screen_y_for_tile(int tile_y) const;
    int tile_width() const;
    int tile_height() const;
    bool is_visible(int tile_x, int tile_y) const;
    bool is_visible_rect(int x, int y, int w, int h) const;
    int get_offset_x() const;
    int get_offset_y() const;
    void zoom_in();
    void zoom_out();
};

#endif
