#include "cargarPersonajeController.h"

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <map>
#include <string>

CargarPersonajeController::CargarPersonajeController(QObject* parent)
    : QObject(parent){}

void CargarPersonajeController::run(DatosConexion& datos, CargarPersonajeResultado& resultado) {
    _volverAlMenu = false;
    _volverACrearCuenta = false;
    resultado = CargarPersonajeResultado::ContinuarConPersonajeExistente;
    errorLoginMessage = datos.getMensajeError();
    errorLogin = datos.tieneErrorLogin();

    // Cargar cargarPersonaje.qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cargarPersonajeController", this);
    engine.load(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/cargarPersonaje/cargarPersonaje.qml")));

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
