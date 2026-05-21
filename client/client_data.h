//
// Created by victoria zubieta on 18/05/2026.
//

#ifndef TALLER_TP_CLIENT_DATA_H
#define TALLER_TP_CLIENT_DATA_H
#include <cstdint>
#include <string>
#include <variant>

enum class Direction : uint8_t { Up, Down, Left, Right, Unknown };
enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
};
#endif //TALLER_TP_CLIENT_DATA_H
