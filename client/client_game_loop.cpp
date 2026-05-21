//
// Created by victoria zubieta on 17/05/2026.
//

#include "client_game_loop.h"

#include <SDL.h>

#include <iostream>
#include <variant>

using namespace SDL2pp;

ClientGameLoop::ClientGameLoop(Queue<ComandoJugador>& outgoing_commands):
    commands_queue(outgoing_commands),
    business(outgoing_commands),
    is_running(false) {}

ClientGameLoop::~ClientGameLoop() = default;

void ClientGameLoop::init(const char* title, int xpos,
    int ypos, int width, int height, bool fullscreen) {
    uint32_t flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    SDL sdl(SDL_INIT_VIDEO);
    Window window(title, xpos, ypos, width, height, flags);
    Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
    window.Raise();
    is_running = true;
    while (is_running) {
        handleEvents();
        update();
    }
    renderer.SetDrawColor(30,30,30,255);
    // Clear screen
    renderer.Clear();

    // Show rendered frame
    renderer.Present();
    SDL_Delay(3000);

}

void ClientGameLoop::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (auto action = handler.handle_event(event)) {
            business.save_command(*action);
        }
    }

    if (handler.should_quit()) {
        is_running = false;
    }
}

// ver como se actualizan los comandos
void ClientGameLoop::update() {
    ComandoJugador command;
    while (commands_queue.try_pop(command)) {
        if (command.opcode == Opcode::MOVER) {
            const auto move = std::get<ComandoMover>(command.payload);
            std::cout << "Move command queued. direction=" << static_cast<int>(move.direccion)
                      << std::endl;
        }
    }
}



bool ClientGameLoop::isRunning() const {
    return is_running;
}
