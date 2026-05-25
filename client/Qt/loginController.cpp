#include "loginController.h"

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QByteArray>
#include <utility>

DatosLogin LoginController::ejecutar() {

    // Cargar login.qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("loginController", this);
    engine.load(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/login.qml")));

    // Corro eventLoop hasta que el usuario complete datos y se emita la señal formularioCompleto
    QEventLoop loop;
    connect(this, &LoginController::loginCompleted, &loop, &QEventLoop::quit);
    
    // Espero a que el eventLoop termine
    loop.exec();

    DatosLogin datos;
    datos.puerto = selectedPuerto.toStdString();
    datos.host = selectedHost.toStdString();
    return datos;
}

LoginController::LoginController(QObject* parent) : QObject(parent) {}

bool LoginController::esPuertoValido(const QString& puerto) const {
    bool ok = false;
    const int valor = puerto.toInt(&ok);
    return valor == 7666;
}

bool LoginController::esHostValido(const QString& host) const {
    return host == "localhost" || host == "127.0.0.1";
}

void LoginController::setPuerto(const QString& puerto) {
    selectedPuerto = puerto;
    if (!selectedPuerto.isEmpty() && !selectedHost.isEmpty()) {
        emit loginCompleted();
    }
}

void LoginController::setHost(const QString& host) {
    selectedHost = host;
    if (!selectedPuerto.isEmpty() && !selectedHost.isEmpty()) {
        emit loginCompleted();
    }
}

QString LoginController::getPuerto() const { return selectedPuerto; }
QString LoginController::getHost() const { return selectedHost; }
