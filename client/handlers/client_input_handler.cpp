//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_input_handler.h"

ClientInputHandler::ClientInputHandler(): quit_requested(false) {}

ClientInputHandler::~ClientInputHandler() = default;

std::optional<GameAction> ClientInputHandler::handle_keyboard(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE:
            quit_requested = true;
            return std::nullopt;
        case SDLK_UP:    case SDLK_w: return GameAction::MoveUp;
        case SDLK_DOWN:  case SDLK_s: return GameAction::MoveDown;
        case SDLK_LEFT:  case SDLK_a: return GameAction::MoveLeft;
        case SDLK_RIGHT: case SDLK_d: return GameAction::MoveRight;
        default: return std::nullopt;
    }
}

std::optional<GameAction> ClientInputHandler::handle_event(const SDL_Event &event) {
    if (event.type == SDL_QUIT) {
        quit_requested = true;
    }
    if (event.type == SDL_KEYDOWN) {
        return handle_keyboard(event.key.keysym.sym);
    }
    return std::nullopt;

}

bool ClientInputHandler::should_quit() const { return quit_requested; }
