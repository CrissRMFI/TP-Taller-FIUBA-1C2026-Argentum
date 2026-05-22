#include <iostream>
#include <exception>
#include <SDL.h>

#include "client__.h"
#include <SDL2pp/Renderer.hh>

#include "client_game_loop.h"
#include "../common/thread/queue.h"
#include "cmake-build-debug/_deps/sdl2-src/include/SDL_video.h"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"

using namespace SDL2pp;

int main(int argc, char* argv[]) try {

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
		return EXIT_FAILURE;
	}
	const char* hostname = argv[1];
	const char* servname = argv[2];
	Client client(hostname, servname);
	client.run();

	return 0;

	}catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return 1;
}

/*
 * Game loop ---> que queremos hacer?
 * manejar inputs cliente/usuario
 * updatear tdo objeto (posicion, vestimenta, etc)
 * render cambia display
 *


	// Here all resources are automatically released and library deinitialized
 */
