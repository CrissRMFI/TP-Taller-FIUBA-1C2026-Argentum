#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H

#include "datosConexion.h"

class QQuickView;

class MenuController{
public:
    MenuController();
    void setVentana(QQuickView* ventana);
    void run(DatosConexion& datos);
    DatosConexion nombreUsuarioNoEncontrado(const DatosLogin& datosLogin);
    DatosConexion nickYaExistente(const DatosLogin& datosLogin);
    DatosConexion puertoHostInvalidos();
    DatosConexion usuarioYaConectado(const DatosLogin& datosLogin);
private:
    QQuickView* ventana = nullptr;
    bool terminoRegistro = false;
    bool loginYaRealizado = false;
};

#endif
