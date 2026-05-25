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
#include "Qt/datosConexion.h"
#include "Qt/cargarPersonajeController.h"
#include "Qt/loginController.h"
#include "Qt/elegirPersonajeController.h"
#include <QGuiApplication>

using namespace SDL2pp;

int main(int argc, char* argv[]) try {
	QGuiApplication app(argc, argv);

	DatosLogin datos;
	DatosNuevoPersonaje nuevoPersonaje;
	DatosPersonaje datosPersonaje;
	bool esNuevoPersonaje = false;
	bool terminoRegistro = false;

	while (!terminoRegistro) {
		LoginController login;
		datos = login.ejecutar();

		while (true) {
			CargarPersonajeController cargarPersonaje;
			const auto cargarResult = cargarPersonaje.ejecutar();
			datosPersonaje = cargarResult.first;
			const bool cargarPersonajeVolverAlMenu = cargarResult.second.first;
			const bool cargarPersonajeVolverACrearPersonaje = cargarResult.second.second;
			if (cargarPersonajeVolverAlMenu) {
				break;
			}
			if (!cargarPersonajeVolverACrearPersonaje) {
				terminoRegistro = true;
				break;
			}

			ElegirPersonajeController elegirPersonaje;
			const auto elegirResult = elegirPersonaje.ejecutar();
			const bool volverAtras = elegirResult.first;
			if (volverAtras) {
				continue;
			}

			nuevoPersonaje = elegirResult.second;
			esNuevoPersonaje = true;
			terminoRegistro = true;
			break;
		}
	}
	if (esNuevoPersonaje) {
		printf("Se eligió crear un nuevo personaje.\n");
		printf("Puerto: %s, Host: %s, Raza: %d, Clase: %d, NewNick: %s, NewPassword: %s\n",
	       datos.puerto.c_str(), datos.host.c_str(),
	       static_cast<int>(nuevoPersonaje.raza), static_cast<int>(nuevoPersonaje.clase),
	       nuevoPersonaje.nick.c_str(), nuevoPersonaje.password.c_str());
	} else {
		printf("Se eligió cargar un personaje existente.\n");
		printf("Puerto: %s, Host: %s, Nick: %s, Password: %s\n",
		       datos.puerto.c_str(), datos.host.c_str(),
		       datosPersonaje.nick.c_str(), datosPersonaje.password.c_str());
	}
	

	// Client client(datos.host.c_str(), datos.puerto.c_str());
	// client.run();

	return 0;
} catch (std::exception& e) {
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
