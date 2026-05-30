#include "menuController.h"
#include "cargarPersonaje/cargarPersonajeController.h"
#include "login/loginController.h"
#include "elegirPersonaje/elegirPersonajeController.h"

#include "../../common/socket/socket.h"

MenuController::MenuController(){}

void MenuController::run(DatosConexion& datos) {

	while (!terminoRegistro) {
		if (!loginYaRealizado) {
			LoginController login;
			login.run(datos);
			try {
				Socket preflight(datos.getDatosLogin().host.c_str(),
				                 datos.getDatosLogin().puerto.c_str());
				preflight.close();
			} catch (...) {
				datos = puertoHostInvalidos();
				return;
			}
			datos.clearError();
		}

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
	terminoRegistro = false;
    return;
}

DatosConexion MenuController::nombreUsuarioNoEncontrado(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::NombreUsuarioNoEncontrado);
	loginYaRealizado = true;
    run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
	return datos;
}

DatosConexion MenuController::nickYaExistente(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::NickYaExistente);
	loginYaRealizado = true;
	run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
	return datos;
}

DatosConexion MenuController::puertoHostInvalidos() {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::PuertoHostInvalidos);
    run(datos);
	return datos;
}

DatosConexion MenuController::usuarioYaConectado(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::UsuarioYaConectado);
	loginYaRealizado = true;
    run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
    return datos;
}

DatosConexion MenuController::passwordIncorrecto(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::PasswordIncorrecto);
	loginYaRealizado = true;
    run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
    return datos;
}
