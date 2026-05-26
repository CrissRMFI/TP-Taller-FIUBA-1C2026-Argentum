#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H

#include "datosConexion.h"
#include "menuController.h"

class ConnectionController {
public:
    ConnectionController();
    int run(int argc, char* argv[]);
private:
    MenuController menu;
    DatosConexion datos;
    bool cuentaNoEncontrada = false;
    bool nickYaExistente = false;
    bool puertoHostInvalidos = false;
    bool usuarioYaConectado = false;
};

#endif