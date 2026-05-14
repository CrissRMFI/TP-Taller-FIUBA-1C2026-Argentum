#include "common/foo.h"

#include <iostream>
#include <exception>

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

#include "client__.h"

#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0

int main(const int argc, const char* argv[]) {


    try {
        if (argc != 3) {
             std::cerr << "Usage: " << argv[0] << " <host> <port> <username>\n";
            return EXIT_FAILURE;
        }


        const char* hostname = argv[1];
    	const char* servname = argv[2];

        Client client(hostname, servname);
        client.run();

        return EXIT_SUCCESS;


    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return EXIT_FAILURE;
    }
}

/*
using namespace SDL2pp;

int main() try {
	// Initialize SDL library
	SDL sdl(SDL_INIT_VIDEO);

	// Create main window: 640x480 dimensions, resizable, "SDL2pp demo" title
	Window window("SDL2pp demo",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			SDL_WINDOW_RESIZABLE);

	// Create accelerated video renderer with default driver
	Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Clear screen
	renderer.Clear();

	// Show rendered frame
	renderer.Present();

	// 5 second delay
	SDL_Delay(5000);

	// Here all resources are automatically released and library deinitialized
	return 0;
} catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return 1;
}
*/