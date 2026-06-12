#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H

#include "datosConexion.h"
#include "gestor_audio_menu.h"

#include <memory>

class QQuickView;

class MenuController {
public:
    MenuController();
    void setVentana(QQuickView* ventana_view);
    void run(DatosConexion& datos);
    DatosConexion nombreUsuarioNoEncontrado(const DatosLogin& datosLogin);
    DatosConexion nickYaExistente(const DatosLogin& datosLogin);
    DatosConexion puertoHostInvalidos();
    DatosConexion usuarioYaConectado(const DatosLogin& datosLogin);
private:
    QQuickView* ventana = nullptr;
    bool terminoRegistro = false;
    bool loginYaRealizado = false;
    std::unique_ptr<GestorAudioMenu> audio;
    void configurarAudio();
    void centrarVentana(QQuickView* ventana_);
};

#endif
