#ifndef CARGARPERSONAJECONTROLLER_H
#define CARGARPERSONAJECONTROLLER_H

#include <qqml.h>
#include <QObject>
#include <QString>

#include "datosConexion.h"

enum class CargarPersonajeResultado {
    VolverAlMenu,
    CrearCuenta,
    ContinuarConPersonajeExistente,
};

class QQuickView;

class CargarPersonajeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString nick READ getNick)
    QML_ELEMENT

public:
    explicit CargarPersonajeController(QObject* parent = nullptr);
    void run(QQuickView& ventana, DatosConexion& datos, CargarPersonajeResultado& resultado);

public slots:
    void setNick(const QString& nick);
    bool esNickValido(const QString& texto) const;
    void volverAlMenu();
    void volverACrearCuenta();

    bool huboErrorLogin() const;
    QString getNick() const;
    QString getErrorLoginMessage() const;

signals:
    void cargarPersonajeCompleted();

private:
    DatosPersonaje datosPersonaje;
    std::string errorLoginMessage;
    bool _volverAlMenu = false;
    bool _volverACrearCuenta = false;
    bool errorLogin = false;
};

#endif
