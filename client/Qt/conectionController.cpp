#include "conectionController.h"

#include <cmath>
#include <iostream>
#include <string>

#include <QColor>
#include <QGuiApplication>
#include <QQuickView>
#include <QSize>

#include "../config/config_cliente.h"
#include "../config/lector_config_cliente.h"
#include "../registro_cliente.h"

#ifndef CLIENT_CONFIG_PATH
#define CLIENT_CONFIG_PATH "config/client_config.toml"
#endif

ConnectionController::ConnectionController() {}

int ConnectionController::run(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    // Ventana unica del menu, del tamano del juego.
    LectorConfigCliente lectorConfig;
    const ConfigCliente cfg = lectorConfig.cargar(CLIENT_CONFIG_PATH);
    const int ventanaAncho = cfg.ancho;
    const int ventanaAlto = cfg.alto;

    QQuickView ventana;
    ventana.setColor(QColor(0, 0, 0));
    ventana.setResizeMode(QQuickView::SizeRootObjectToView);
    ventana.setTitle("Argentum Online");
    ventana.setWidth(ventanaAncho);
    ventana.setHeight(ventanaAlto);
    ventana.setMinimumSize(QSize(ventanaAncho, ventanaAlto));
    ventana.setMaximumSize(QSize(ventanaAncho, ventanaAlto));
    menu.setVentana(&ventana);

    menu.run(datos);

    while (true) {
        ventana.hide();
        try {
            Client client(datos.getDatosLogin().host.c_str(), datos.getDatosLogin().puerto.c_str(),
                          datos);
            RegistroCliente::info("Conexion exitosa al servidor en " + datos.getDatosLogin().host +
                                  ":" + datos.getDatosLogin().puerto + ". Corriendo el cliente...");
            client.run();
            return 0;

        } catch (const HandshakeError& e) {
            switch (e.getCode()) {
                case ErrorUsuario::NombreUsuarioNoEncontrado:
                    nombreUsuarioNoEncontrado = true;
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
            RegistroCliente::error("Error de conexion: " + errorMsg);
            puertoHostInvalidos = true;
        }
        if (nombreUsuarioNoEncontrado) {
            datos = menu.nombreUsuarioNoEncontrado(datos.getDatosLogin());
            nombreUsuarioNoEncontrado = false;
        } else if (nickYaExistente) {
            datos = menu.nickYaExistente(datos.getDatosLogin());
            nickYaExistente = false;
        } else if (puertoHostInvalidos) {
            datos = menu.puertoHostInvalidos();
            puertoHostInvalidos = false;
        } else if (usuarioYaConectado) {
            datos = menu.usuarioYaConectado(datos.getDatosLogin());
            usuarioYaConectado = false;
        }
    }

    return 0;
}
