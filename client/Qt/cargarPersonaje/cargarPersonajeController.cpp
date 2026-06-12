#include "cargarPersonajeController.h"

#include <QEventLoop>
#include <QQmlContext>
#include <QQuickView>
#include <QUrl>
#include <map>
#include <string>

CargarPersonajeController::CargarPersonajeController(QObject* parent)
    : QObject(parent){}

void CargarPersonajeController::run(QQuickView& ventana, DatosConexion& datos, CargarPersonajeResultado& resultado) {
    _volverAlMenu = false;
    _volverACrearCuenta = false;
    resultado = CargarPersonajeResultado::ContinuarConPersonajeExistente;
    errorLoginMessage = datos.getMensajeError();
    errorLogin = datos.tieneErrorLogin();

    // Reemplaza el contenido de la ventana compartida por cargarPersonaje.qml.
    ventana.rootContext()->setContextProperty("cargarPersonajeController", this);
    ventana.setSource(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/cargarPersonaje/cargarPersonaje.qml")));
    ventana.show();
    // Corro eventLoop hasta que el usuario complete datos y se emita la señal cargarPersonajeCompleted
    QEventLoop loop;
    connect(this, &CargarPersonajeController::cargarPersonajeCompleted, &loop, &QEventLoop::quit);

    // Espero a que el eventLoop termine
    loop.exec();

    datos.setDatosPersonaje(datosPersonaje.nick);
    if (_volverAlMenu) {
        resultado = CargarPersonajeResultado::VolverAlMenu;
    } else if (_volverACrearCuenta) {
        resultado = CargarPersonajeResultado::CrearCuenta;
    }
}

void CargarPersonajeController::volverAlMenu() {
    _volverAlMenu = true;
    emit cargarPersonajeCompleted();
}

void CargarPersonajeController::volverACrearCuenta() {
    _volverACrearCuenta = true;
    emit cargarPersonajeCompleted();
}

void CargarPersonajeController::setNick(const QString& nick) {
    datosPersonaje.nick = nick.toStdString();
    if (!datosPersonaje.nick.empty()) {
        emit cargarPersonajeCompleted();
    }
}

bool CargarPersonajeController::esNickValido(const QString& nick) const {
    return nick.toUtf8().size() <= 32 && !nick.contains(' ');
}

bool CargarPersonajeController::huboErrorLogin() const {
    return errorLogin;
}

QString CargarPersonajeController::getNick() const { return QString::fromStdString(datosPersonaje.nick); }
QString CargarPersonajeController::getErrorLoginMessage() const {
    return QString::fromStdString(errorLoginMessage);
}
