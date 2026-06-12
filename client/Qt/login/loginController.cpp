#include "loginController.h"

#include <QEventLoop>
#include <QQmlContext>
#include <QQuickView>
#include <QUrl>
#include <QByteArray>
#include <utility>

void LoginController::run(QQuickView& ventana, DatosConexion& datos) {
    errorLogin = datos.tieneErrorLogin();
    errorLoginMessage = QString::fromStdString(datos.getMensajeError());
    ventana.rootContext()->setContextProperty("loginController", this);
    ventana.setSource(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/login/login.qml")));
    ventana.show();

    // Corro eventLoop hasta que el usuario complete datos y se emita la señal loginCompleted
    QEventLoop loop;
    connect(this, &LoginController::loginCompleted, &loop, &QEventLoop::quit);

    // Espero a que el eventLoop termine
    loop.exec();

    datos.setDatosLogin(selectedPuerto.toStdString(), selectedHost.toStdString());
}

LoginController::LoginController(QObject* parent) : QObject(parent) {}

bool LoginController::esPuertoValido(const QString& puerto) const {
    // Verificar que el puerto sea un número y esté en el rango válido
    bool ok;    
    int puertoNum = puerto.toInt(&ok);
    return ok && puertoNum > 0 && puertoNum <= 65535;
}

bool LoginController::esHostIpValido(const QString& hostIp) const {
    return !hostIp.trimmed().isEmpty();
}

bool LoginController::huboErrorLogin() const {
    return errorLogin;
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
QString LoginController::getErrorLogin() const { return errorLoginMessage; }