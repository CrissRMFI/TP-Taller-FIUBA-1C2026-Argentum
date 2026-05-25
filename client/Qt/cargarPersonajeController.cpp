#include "cargarPersonajeController.h"

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <map>
#include <string>

CargarPersonajeController::CargarPersonajeController(QObject* parent)
    : QObject(parent){}

std::pair<DatosPersonaje, std::pair<bool, bool>> CargarPersonajeController::ejecutar() {
    _volverAlMenu = false;
    _volverACrearPersonaje = false;

    // Cargar cargarPersonaje.qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cargarPersonajeController", this);
    engine.load(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/cargarPersonaje.qml")));

    // Corro eventLoop hasta que el usuario complete datos y se emita la señal cargarPersonajeCompleted
    QEventLoop loop;
    connect(this, &CargarPersonajeController::cargarPersonajeCompleted, &loop, &QEventLoop::quit);

    // Espero a que el eventLoop termine
    loop.exec();

    DatosPersonaje datos;
    datos.nick = datosPersonaje.nick;
    datos.password = datosPersonaje.password;
    return {datos, {_volverAlMenu, _volverACrearPersonaje}};
}

void CargarPersonajeController::volverAlMenu() {
    _volverAlMenu = true;
    emit cargarPersonajeCompleted();
}

void CargarPersonajeController::volverACrearPersonaje() {
    _volverACrearPersonaje = true;
    emit cargarPersonajeCompleted();
}

void CargarPersonajeController::setNick(const QString& nick) {
    datosPersonaje.nick = nick.toStdString();
    if (!datosPersonaje.nick.empty() && !datosPersonaje.password.empty()) {
        emit cargarPersonajeCompleted();
    }
}

void CargarPersonajeController::setPassword(const QString& password) {
    datosPersonaje.password = password.toStdString();
    if (!datosPersonaje.nick.empty() && !datosPersonaje.password.empty()) {
        emit cargarPersonajeCompleted();
    }
}

bool CargarPersonajeController::esTextoValido(const QString& texto) const {
    return texto.toUtf8().size() <= 32 && !texto.contains(' ');
}

QString CargarPersonajeController::getNick() const { return QString::fromStdString(datosPersonaje.nick); }
QString CargarPersonajeController::getPassword() const { return QString::fromStdString(datosPersonaje.password); }
