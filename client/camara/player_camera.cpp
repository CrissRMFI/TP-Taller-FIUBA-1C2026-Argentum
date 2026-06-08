//
// Created by victoria zubieta on 07/06/2026.
//

#include "player_camera.h"

#include <algorithm>

namespace {
constexpr int MIN_VISIBLE_TILES_X = 12;
constexpr int MIN_VISIBLE_TILES_Y = 10;
constexpr int MAX_VISIBLE_TILES_X = 60;
constexpr int MAX_VISIBLE_TILES_Y = 50;
constexpr int ZOOM_STEP_TILES = 2;
}  // namespace

void PlayerCamera::recalculate_scale() {
    cell_width = std::max(1, viewport_width / visible_tiles_x);
    cell_height = std::max(1, viewport_height / visible_tiles_y);
    map_pixel_width = map_width_tiles * cell_width;
    map_pixel_height = map_height_tiles * cell_height;
}

void PlayerCamera::configure(const int view_width, const int view_height,
                             const int map_width, const int map_height) {
    viewport_width = view_width;
    viewport_height = view_height;
    map_width_tiles = map_width;
    map_height_tiles = map_height;
    recalculate_scale();
}

/*
 *  std::clamp comparo limites (v, low_ref, high_ref) :
        * si v < low_ref, v = low_ref ;
        * si v > high_ref, v = high_ref;
        * cualquier otro caso: v = v
*/

void PlayerCamera::center_on_tile(const int tile_x, const int tile_y) {
    const int desired_offset_x =
            viewport_width / 2 - (tile_x * cell_width + cell_width / 2);
    const int desired_offset_y =
            viewport_height / 2 - (tile_y * cell_height + cell_height / 2);
    const int min_offset_x = viewport_width - map_pixel_width;
    const int min_offset_y = viewport_height - map_pixel_height;
    const int max_offset_x = 0;
    const int max_offset_y = 0;

    if (map_pixel_width <= viewport_width) {
        offset_x = (viewport_width - map_pixel_width) / 2;
    } else {
        offset_x = std::clamp(desired_offset_x, min_offset_x, max_offset_x);
    }

    if (map_pixel_height <= viewport_height) {
        offset_y = (viewport_height - map_pixel_height) / 2;
    } else {
        offset_y = std::clamp(desired_offset_y, min_offset_y, max_offset_y);
    }
}

void PlayerCamera::center_on_point(const double tile_x, const double tile_y) {
    // Igual que center_on_tile pero con posicion fraccionaria (interpolada): la camara
    // sigue al jugador suave entre celdas, sin saltos de un tile completo.
    const int desired_offset_x =
            viewport_width / 2 - static_cast<int>(tile_x * cell_width + cell_width / 2.0);
    const int desired_offset_y =
            viewport_height / 2 - static_cast<int>(tile_y * cell_height + cell_height / 2.0);
    const int min_offset_x = viewport_width - map_pixel_width;
    const int min_offset_y = viewport_height - map_pixel_height;

    if (map_pixel_width <= viewport_width) {
        offset_x = (viewport_width - map_pixel_width) / 2;
    } else {
        offset_x = std::clamp(desired_offset_x, min_offset_x, 0);
    }

    if (map_pixel_height <= viewport_height) {
        offset_y = (viewport_height - map_pixel_height) / 2;
    } else {
        offset_y = std::clamp(desired_offset_y, min_offset_y, 0);
    }
}

int PlayerCamera::screen_x_for_tile(const int tile_x) const {
    return tile_x * cell_width + offset_x;
}

int PlayerCamera::screen_y_for_tile(const int tile_y) const {
    return tile_y * cell_height + offset_y;
}

int PlayerCamera::tile_width() const {
    return cell_width;
}

int PlayerCamera::tile_height() const {
    return cell_height;
}

// para saber si una celda del mapa esta dentro de la vista
bool PlayerCamera::is_visible(const int tile_x, const int tile_y) const {
    const int screen_x = screen_x_for_tile(tile_x);
    const int screen_y = screen_y_for_tile(tile_y);
    return screen_x + cell_width > 0 && screen_x < viewport_width &&
        screen_y + cell_height > 0 && screen_y < viewport_height;
}

// aprece rectangulo un poco en la pantalla
bool PlayerCamera::is_visible_rect(const int x, const int y, const int w, const int h) const {
    return x + w > 0 &&
           y + h > 0 &&
           x < viewport_width &&
           y < viewport_height;
}

int PlayerCamera::get_offset_x() const {
    return offset_x;
}
int PlayerCamera::get_offset_y () const {
    return offset_y;
}

void PlayerCamera::zoom_in() {
    visible_tiles_x = std::max(MIN_VISIBLE_TILES_X, visible_tiles_x - ZOOM_STEP_TILES);
    visible_tiles_y = std::max(MIN_VISIBLE_TILES_Y, visible_tiles_y - ZOOM_STEP_TILES);
    recalculate_scale();
}

void PlayerCamera::zoom_out() {
    visible_tiles_x = std::min(MAX_VISIBLE_TILES_X, visible_tiles_x + ZOOM_STEP_TILES);
    visible_tiles_y = std::min(MAX_VISIBLE_TILES_Y, visible_tiles_y + ZOOM_STEP_TILES);
    recalculate_scale();
}
