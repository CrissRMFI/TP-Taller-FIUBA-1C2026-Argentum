#include "elegirPersonajeController.h"

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <map>
#include <string>

#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

const std::map<std::string, Raza> razaMap = {
    {"HUMANO", Raza::HUMANO},
    {"ELFO", Raza::ELFO},
    {"ENANO", Raza::ENANO},
    {"GNOMO", Raza::GNOMO}
};

const std::map<std::string, ClasePersonaje> claseMap = {
    {"GUERRERO", ClasePersonaje::GUERRERO},
    {"CLERIGO", ClasePersonaje::CLERIGO},
    {"MAGO", ClasePersonaje::MAGO},
    {"PALADIN", ClasePersonaje::PALADIN}
};

ElegirPersonajeController::ElegirPersonajeController(QObject* parent)
    : QObject(parent) {}

std::pair<bool, DatosNuevoPersonaje> ElegirPersonajeController::ejecutar() {
    _volverAlMenu = false;

    // Cargar elegirPersonaje.qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("personajeController", this);
    engine.load(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/elegirPersonaje.qml")));

    // Corro eventLoop hasta que el usuario complete datos y se emita la señal elegirPersonajeCompleted
    QEventLoop loop;
    connect(this, &ElegirPersonajeController::elegirPersonajeCompleted, &loop, &QEventLoop::quit);
    
    // Espero a que el eventLoop termine
    loop.exec();

    DatosNuevoPersonaje datos;
    datos.raza = selectedRaza;
    datos.clase = selectedClase;
    datos.nick = selectedNick.toStdString();
    datos.password = selectedPassword.toStdString();
    return {_volverAlMenu, datos};
}

void ElegirPersonajeController::volverAlMenu() {
    _volverAlMenu = true;
    emit elegirPersonajeCompleted();
}

void ElegirPersonajeController::setRaza(const QString& raza) {
    auto it = razaMap.find(raza.toStdString());
    if (it != razaMap.end()) {
        selectedRaza = it->second;
    }
    razaSeleccionada = true;
    if (razaSeleccionada && claseSeleccionada && !selectedNick.isEmpty() && !selectedPassword.isEmpty()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setClase(const QString& clase) {
    auto it = claseMap.find(clase.toStdString());
    if (it != claseMap.end()) {
        selectedClase = it->second;
    }
    claseSeleccionada = true;
    if (razaSeleccionada && claseSeleccionada && !selectedNick.isEmpty() && !selectedPassword.isEmpty()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setNick(const QString& nick) {
    selectedNick = nick;
    if (razaSeleccionada && claseSeleccionada && !selectedNick.isEmpty() && !selectedPassword.isEmpty()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setPassword(const QString& password) {
    selectedPassword = password;
    if (razaSeleccionada && claseSeleccionada && !selectedNick.isEmpty() && !selectedPassword.isEmpty()) {
        emit elegirPersonajeCompleted();
    }
}

bool ElegirPersonajeController::esTextoValido(const QString& text) const {
    return text.toUtf8().size() <= 32 && !text.contains(' ');
}

Raza ElegirPersonajeController::getRaza() const { return selectedRaza; }
ClasePersonaje ElegirPersonajeController::getClase() const { return selectedClase; }
QString ElegirPersonajeController::getNick() const { return selectedNick; }
QString ElegirPersonajeController::getPassword() const { return selectedPassword; }