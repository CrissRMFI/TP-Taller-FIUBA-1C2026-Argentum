#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H

#include <memory>

#include "datosConexion.h"
#include "menuController.h"
#include "../client__.h"
#include "../handshake_error.h"

class ConnectionController {
public:
    ConnectionController();
    int run(int argc, char* argv[]);
private:
    MenuController menu;
    DatosConexion datos;
    bool nombreUsuarioNoEncontrado = false;
    bool nickYaExistente = false;
    bool puertoHostInvalidos = false;
    bool usuarioYaConectado = false;
    bool passwordIncorrecto = false;
};

#endif