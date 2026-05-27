#include "conectionController.h"

#include <QGuiApplication>
#include <iostream>
#include <string>

#include "../client__.h"
#include "../handshake_error.h"

ConnectionController::ConnectionController() {}

int ConnectionController::run(int argc, char* argv[]){
	QGuiApplication app(argc, argv);
	menu.run(datos);

	while(true) {
		try{		
			Client client(datos.getDatosLogin().host.c_str(), datos.getDatosLogin().puerto.c_str(), datos);
			printf("Conexión exitosa al servidor en %s:%s\n Corriendo el cliente...", datos.getDatosLogin().host.c_str(), datos.getDatosLogin().puerto.c_str());
			client.run();
			return 0;
			
		} catch (const HandshakeError& e) {
			switch (e.getCode()) {
				case ErrorUsuario::CuentaNoEncontrada:
					cuentaNoEncontrada = true;
					break;
				case ErrorUsuario::NickYaExistente:
					nickYaExistente = true;
					break;
				case ErrorUsuario::UsuarioYaConectado:
					usuarioYaConectado = true;
			        break;
				case ErrorUsuario::Ninguno:
					break;
			}
		} catch (const std::exception& e) {
			std::string errorMsg = e.what();
			// Error desconocido, se asume que es un error de conexión (puerto/host inválidos)
			printf("Error de conexión: %s\n", errorMsg.c_str());
			puertoHostInvalidos = true;
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
		} else if (usuarioYaConectado) {
			datos = menu.usuarioYaConectado();
			usuarioYaConectado = false;
		}
	}

	return 0;
}
