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

    // Cargar cargarPersonaje.qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cargarPersonajeController", this);
    engine.load(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/cargarPersonaje/cargarPersonaje.qml")));

    // Corro eventLoop hasta que el usuario complete datos y se emita la señal cargarPersonajeCompleted
    QEventLoop loop;
    connect(this, &CargarPersonajeController::cargarPersonajeCompleted, &loop, &QEventLoop::quit);

    // Espero a que el eventLoop termine
    loop.exec();

    datos.setDatosPersonaje(datosPersonaje.nick, datosPersonaje.password);
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
