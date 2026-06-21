//
// Created by victoria zubieta on 18/05/2026.
//

#ifndef TALLER_TP_CLIENT_DATA_H
#define TALLER_TP_CLIENT_DATA_H
#include <cstdint>
#include <map>
#include <string>
#include <variant>

enum class Direction : uint8_t { Up, Down, Left, Right, Unknown };
enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
};


constexpr int animation_row_for_action(const GameAction action) {
    switch (action) {
        case GameAction::MoveDown:
            return 0;
        case GameAction::MoveLeft:
            return 1;
        case GameAction::MoveRight:
            return 2;
        case GameAction::MoveUp:
            return 3;
    }

    return 0;
}


constexpr uint8_t direction_for_protocol(const GameAction action) {
    switch (action) {
        case GameAction::MoveDown:
            return 0;
        case GameAction::MoveUp:
            return 1;
        case GameAction::MoveLeft:
            return 3;
        case GameAction::MoveRight:
            return 2;
    }

    return 0;
}

#endif  // TALLER_TP_CLIENT_DATA_H
