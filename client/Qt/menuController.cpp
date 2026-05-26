#include "menuController.h"
#include "datosConexion.h"
#include "cargarPersonaje/cargarPersonajeController.h"
#include "login/loginController.h"
#include "elegirPersonaje/elegirPersonajeController.h"

MenuController::MenuController(){}

void MenuController::run(DatosConexion& datos) {
	bool terminoRegistro = false;

	while (!terminoRegistro) {
		LoginController login;
		login.run(datos);

		while (true) {
			CargarPersonajeController cargarPersonaje;
			CargarPersonajeResultado cargarResultado;
			cargarPersonaje.run(datos, cargarResultado);

			if (cargarResultado == CargarPersonajeResultado::VolverAlMenu) {
				break;
			}

			if (cargarResultado == CargarPersonajeResultado::ContinuarConPersonajeExistente) {
				terminoRegistro = true;
				break;
			}

			ElegirPersonajeController elegirPersonaje;
			ElegirPersonajeResultado elegirResultado;
			elegirPersonaje.run(datos, elegirResultado);

			if (elegirResultado == ElegirPersonajeResultado::VolverAlMenu) {
				continue;
			}

			terminoRegistro = true;
			break;
		}
	}
    return;
}

DatosConexion MenuController::cuentaNoEncontrada() {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::CuentaNoEncontrada);
	LoginController login;
    run(datos);
	return datos;
}

DatosConexion MenuController::nickYaExistente() {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::NickYaExistente);
	LoginController login;
	run(datos);
	return datos;
}

DatosConexion MenuController::puertoHostInvalidos() {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::PuertoHostInvalidos);
	LoginController login;
    run(datos);
	return datos;
}