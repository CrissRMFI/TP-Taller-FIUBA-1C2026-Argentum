#ifndef CARGARPERSONAJECONTROLLER_H
#define CARGARPERSONAJECONTROLLER_H

#include <QObject>
#include <QString>
#include <qqml.h>

#include "datosConexion.h"

enum class CargarPersonajeResultado {
    VolverAlMenu,
    CrearCuenta,
    ContinuarConPersonajeExistente,
};

class CargarPersonajeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString nick READ getNick)
    Q_PROPERTY(QString password READ getPassword)
    QML_ELEMENT

public:
    explicit CargarPersonajeController(QObject* parent = nullptr);
    void run(DatosConexion& datos, CargarPersonajeResultado& resultado);

public slots:
    void setNick(const QString& nick);
    void setPassword(const QString& password);
    bool esTextoValido(const QString& texto) const;
    void volverAlMenu();
    void volverACrearCuenta();

    bool huboErrorLogin() const;

    QString getNick() const;
    QString getPassword() const;
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