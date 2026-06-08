//
// Created by victoria zubieta on 07/06/2026.
//

#include "player_camera.h"


void PlayerCamera::configure(const int view_width, const int view_height,
                             const int map_width, const int map_height) {
    viewport_width = view_width;
    viewport_height = view_height;
    cell_width = viewport_width / map_width;
    cell_height =  viewport_height / map_height;
}

void PlayerCamera::center_on_tile(const int tile_x, const int tile_y) {
    offset_x = viewport_width / 2 - (tile_x * cell_width + cell_width / 2);
    offset_y = viewport_height / 2 - (tile_y * cell_height + cell_height / 2);
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
bool PlayerCamera::is_visible(const int tile_x, const int tile_y) const {
    const int screen_x = screen_x_for_tile(tile_x);
    const int screen_y = screen_y_for_tile(tile_y);
    return screen_x + cell_width > 0 && screen_x < viewport_width &&
        screen_y + cell_height > 0 && screen_y < viewport_height;
}