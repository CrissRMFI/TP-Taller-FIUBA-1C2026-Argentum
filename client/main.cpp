#include <iostream>
#include <exception>
#include <SDL.h>
#include <cstdio>

#include "client__.h"
#include <SDL2pp/Renderer.hh>

#include "client_game_loop.h"
#include "../common/thread/queue.h"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"
#include "Qt/conectionController.h"

using namespace SDL2pp;

int main(int argc, char* argv[]){
	try
	{
		ConnectionController connectionController;
		return connectionController.run(argc, argv);
	}
	catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << std::endl;
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
}
