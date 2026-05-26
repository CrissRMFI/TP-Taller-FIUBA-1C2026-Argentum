#include "conectionController.h"

#include <QGuiApplication>
#include <iostream>
#include <string>

#include "../client__.h"

ConnectionController::ConnectionController() {}

int ConnectionController::run(int argc, char* argv[]){
	QGuiApplication app(argc, argv);
	menu.run(datos);

	while(pedirDatos) {
		try{		
			Client client(datos.getDatosLogin().host.c_str(), datos.getDatosLogin().puerto.c_str(), datos);
			client.run();
			return 0;
			
		} catch (const std::exception& e) {
			std::string errorMsg = e.what();
			if (errorMsg.find("Cuenta no encontrada") != std::string::npos) {
				cuentaNoEncontrada = true;
			} else if (errorMsg.find("Nick ya existente") != std::string::npos) {
				nickYaExistente = true;
			} else {
				// Error desconocido, se asume que es un error de conexión (puerto/host inválidos)
				puertoHostInvalidos = true;
			}
		}
		if (cuentaNoEncontrada) {
			datos = menu.cuentaNoEncontrada();
			cuentaNoEncontrada = false;
		} else if (nickYaExistente) {
			datos = menu.nickYaExistente();
			nickYaExistente = false;
		} else if (puertoHostInvalidos) {
			datos = menu.puertoHostInvalidos();
			puertoHostInvalidos = false;
		}
	}

	return 0;
}
