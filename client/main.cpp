#include <iostream>
#include <exception>
#include <SDL.h>

#include "client__.h"
#include <SDL2pp/Renderer.hh>

#include "client_game_loop.h"
#include "../common/thread/queue.h"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"

using namespace SDL2pp;

int main() try {

	// if (argc != 3) {
	// 	std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
	// 	return EXIT_FAILURE;
	// }
	// Initialize SDL library
		Queue<ComandoJugador> commands;
		ClientGameLoop game_loop(commands);
		game_loop.init("Argentum - Parte I",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			640, 480, false);


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

// const char* hostname = argv[1];
	// const char* servname = argv[2];
	//
	// Client client(hostname, servname);
	// client.run();
	//
	// Here all resources are automatically released and library deinitialized
 */
