//
// Created by victoria zubieta on 07/06/2026.
//

#include "player_camera.h"

#include <algorithm>
/*
 * cuuntas tiles del mapa quiero mostrar en la camara del jugador
 * cuanto mas grande las tiles => menos zoom
 * mas chicas las tiles  => mas zoom
*/
#define VISIBLE_TILES_X  35 // --> cuantas tiles del mapa en x
#define VISIBLE_TILES_Y  30 // --> cuantas tiles del mapa en y

void PlayerCamera::configure(const int view_width, const int view_height,
                             const int map_width, const int map_height) {
    viewport_width = view_width;
    viewport_height = view_height;
    cell_width = std::max(1, viewport_width / static_cast<int>(VISIBLE_TILES_X));
    cell_height = std::max(1, viewport_height / static_cast<int>(VISIBLE_TILES_Y));
    map_pixel_width = map_width * cell_width;
    map_pixel_height = map_height * cell_height;
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
