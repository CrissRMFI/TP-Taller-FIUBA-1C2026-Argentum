
#include "client_game_loop.h"

#include <SDL.h>
#include <SDL_image.h>

#define FRAME_DELAY_MS 10

ClientGameLoop::ClientGameLoop(Queue<MensajeServidor>& server_messages,
                               ClientBusiness& business,
                               const uint16_t idCliente):
    server_messages(server_messages),
    business(business),
    object_state(idCliente),
    is_running(false) {}

ClientGameLoop::~ClientGameLoop() = default;

void ClientGameLoop::init(const char* title,
                          const int xpos,
                          const int ypos,
                          const int width,
                          const int height,
                          const bool fullscreen) {
    object_renderer.init(title, xpos, ypos, width, height, fullscreen);

    is_running = true;
    while (is_running) {
        handleEvents();
        update();
        render();
        SDL_Delay(FRAME_DELAY_MS);
    }
}

void ClientGameLoop::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (auto action = handler.handle_event(event)) {
            object_animation.on_action(*action);
            object_state.notify_move_requested(SDL_GetTicks());
            business.save_command(*action);
        }
    }
    if (handler.should_quit()) {
        is_running = false;
    }
}

void ClientGameLoop::update() {
    const uint32_t current_tick = SDL_GetTicks();
    object_state.upload_server_msg(server_messages, current_tick);
    object_renderer.update_animation(current_tick, object_state, object_animation);
}

void ClientGameLoop::render() {
    object_renderer.render(object_state, object_animation);
}

void ClientGameLoop::clean() {}

bool ClientGameLoop::isRunning() const {
    return is_running;
}