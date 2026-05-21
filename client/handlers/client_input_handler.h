//
// Created by victoria zubieta on 17/05/2026.
//

#ifndef TALLER_TP_CLIENT_INPUT_HANDLER_H
#define TALLER_TP_CLIENT_INPUT_HANDLER_H

#include <optional>

#include <SDL.h>

#include "../client_data.h"

class ClientInputHandler {
private:
    bool quit_requested;
    std::optional<GameAction> handle_keyboard(SDL_Keycode key);

public:
    ClientInputHandler();
    ~ClientInputHandler();

    std::optional<GameAction>  handle_event(const SDL_Event &event);
    bool should_quit() const;
};


#endif //TALLER_TP_CLIENT_INPUT_HANDLER_H
