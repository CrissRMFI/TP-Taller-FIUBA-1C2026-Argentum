#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H

#include "datosConexion.h"


class MenuController{
public:
    MenuController();
    void run(DatosConexion& datos);
    DatosConexion cuentaNoEncontrada();
    DatosConexion nickYaExistente();
    DatosConexion puertoHostInvalidos();
};

#endif