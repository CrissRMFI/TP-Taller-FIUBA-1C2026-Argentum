#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H

#include "datosConexion.h"


class MenuController{
public:
    MenuController();
    void run(DatosConexion& datos);
    DatosConexion nombreUsuarioNoEncontrado(const DatosLogin& datosLogin);
    DatosConexion nickYaExistente(const DatosLogin& datosLogin);
    DatosConexion puertoHostInvalidos();
    DatosConexion usuarioYaConectado(const DatosLogin& datosLogin);
    DatosConexion passwordIncorrecto(const DatosLogin& datosLogin);
private:
    bool terminoRegistro = false;
    bool loginYaRealizado = false;
};

#endif
