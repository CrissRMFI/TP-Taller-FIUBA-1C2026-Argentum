#include <SDL.h>
#include <cstdio>
#include <exception>
#include <iostream>

#include <SDL2pp/Renderer.hh>
#include "client__.h"

#include "../common/thread/queue.h"
#include "Qt/conectionController.h"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"
#include "client_game_loop.h"
#include "registro_cliente.h"

#include <string>

using namespace SDL2pp;

int main(int argc, char* argv[]) {
    try {
        ConnectionController connectionController;
        return connectionController.run(argc, argv);
    } catch (const std::exception& e) {
        RegistroCliente::error(std::string("Error fatal: ") + e.what());
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
