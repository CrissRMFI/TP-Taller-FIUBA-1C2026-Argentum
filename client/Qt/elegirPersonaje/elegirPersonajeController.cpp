#include "elegirPersonajeController.h"

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <map>
#include <string>

#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

namespace {
Raza parseRaza(const QString& raza) {
    if (raza.compare("Humano", Qt::CaseInsensitive) == 0 ||
        raza.compare("HUMANO", Qt::CaseInsensitive) == 0) {
        return Raza::HUMANO;
    }
    if (raza.compare("Elfo", Qt::CaseInsensitive) == 0 ||
        raza.compare("ELFO", Qt::CaseInsensitive) == 0) {
        return Raza::ELFO;
    }
    if (raza.compare("Enano", Qt::CaseInsensitive) == 0 ||
        raza.compare("ENANO", Qt::CaseInsensitive) == 0) {
        return Raza::ENANO;
    }
    return Raza::GNOMO;
}

ClasePersonaje parseClase(const QString& clase) {
    if (clase.compare("Mago", Qt::CaseInsensitive) == 0 ||
        clase.compare("MAGO", Qt::CaseInsensitive) == 0) {
        return ClasePersonaje::MAGO;
    }
    if (clase.compare("Paladín", Qt::CaseInsensitive) == 0 ||
        clase.compare("Paladin", Qt::CaseInsensitive) == 0 ||
        clase.compare("PALADIN", Qt::CaseInsensitive) == 0) {
        return ClasePersonaje::PALADIN;
    }
    if (clase.compare("Clérigo", Qt::CaseInsensitive) == 0 ||
        clase.compare("Clerigo", Qt::CaseInsensitive) == 0 ||
        clase.compare("CLERIGO", Qt::CaseInsensitive) == 0) {
        return ClasePersonaje::CLERIGO;
    }
    return ClasePersonaje::GUERRERO;
}
}  // namespace

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
        selectedCuerpo);
    if (_volverAlMenu) {
        resultado = ElegirPersonajeResultado::VolverAlMenu;
    }
}

void ElegirPersonajeController::volverAlMenu() {
    _volverAlMenu = true;
    emit elegirPersonajeCompleted();
}

void ElegirPersonajeController::setRaza(const QString& raza) {
    selectedRaza = parseRaza(raza);
    razaSeleccionada = true;
    if (camposCompletos()) {
        emit elegirPersonajeCompleted();
    }
}

void ElegirPersonajeController::setClase(const QString& clase) {
    selectedClase = parseClase(clase);
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
    return razaSeleccionada && claseSeleccionada && !selectedNick.isEmpty() &&
           !selectedPassword.isEmpty() && cabezaSeleccionada && cuerpoSeleccionado;
}

Raza ElegirPersonajeController::getRaza() const { return selectedRaza; }
ClasePersonaje ElegirPersonajeController::getClase() const { return selectedClase; }
QString ElegirPersonajeController::getNick() const { return selectedNick; }
QString ElegirPersonajeController::getPassword() const { return selectedPassword; }
