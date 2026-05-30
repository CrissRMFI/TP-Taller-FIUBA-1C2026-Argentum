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

void ElegirPersonajeController::run(DatosConexion& datos, ElegirPersonajeResultado& resultado) {
    _volverAlMenu = false;
    resultado = ElegirPersonajeResultado::FinalizarRegistro;

    // Cargar elegirPersonaje.qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("personajeController", this);
    engine.load(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/elegirPersonaje/elegirPersonaje.qml")));

    // Corro eventLoop hasta que el usuario complete datos y se emita la señal elegirPersonajeCompleted
    QEventLoop loop;
    connect(this, &ElegirPersonajeController::elegirPersonajeCompleted, &loop, &QEventLoop::quit);
    
    // Espero a que el eventLoop termine
    loop.exec();

    datos.setDatosNuevoPersonaje(
        selectedNick.toStdString(),
        selectedPassword.toStdString(),
        selectedRaza,
        selectedClase,
        selectedCabeza,
        selectedCuerpo
    );
    if (_volverAlMenu) {
        resultado = ElegirPersonajeResultado::VolverAlMenu;
    }
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
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setClase(const QString& clase) {
    auto it = claseMap.find(clase.toStdString());
    if (it != claseMap.end()) {
        selectedClase = it->second;
    }
    claseSeleccionada = true;
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setNick(const QString& nick) {
    selectedNick = nick;
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setPassword(const QString& password) {
    selectedPassword = password;
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setCabeza(int cabezaIndex) {
    selectedCabeza = cabezaIndex;
    cabezaSeleccionada = true;
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setCuerpo(int cuerpoIndex) {
    selectedCuerpo = cuerpoIndex;
    cuerpoSeleccionado = true;
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

bool ElegirPersonajeController::esTextoValido(const QString& text) const {
    return text.toUtf8().size() <= 32 && !text.contains(' ');
}

bool ElegirPersonajeController::camposCompletos() const {
    return razaSeleccionada && claseSeleccionada && !selectedNick.isEmpty() && !selectedPassword.isEmpty() && cabezaSeleccionada && cuerpoSeleccionado;
}

Raza ElegirPersonajeController::getRaza() const { return selectedRaza; }
ClasePersonaje ElegirPersonajeController::getClase() const { return selectedClase; }
QString ElegirPersonajeController::getNick() const { return selectedNick; }
QString ElegirPersonajeController::getPassword() const { return selectedPassword; }