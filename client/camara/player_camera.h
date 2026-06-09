//
// Created by victoria zubieta on 07/06/2026.
//

#ifndef TALLER_TP_PLAYER_CAMERA_H
#define TALLER_TP_PLAYER_CAMERA_H

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
    int visible_tiles_x = 35;
    int visible_tiles_y = 30;

    void recalculate_scale();

public:
    void configure(int view_width, int view_height, int map_width, int map_height);
    void center_on_tile(int tile_x, int tile_y);
    void center_on_point(double tile_x, double tile_y);  // centrado suave (posicion interpolada)
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

#endif  // TALLER_TP_PLAYER_CAMERA_H
