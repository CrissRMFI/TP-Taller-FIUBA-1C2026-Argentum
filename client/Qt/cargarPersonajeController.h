#ifndef CARGARPERSONAJECONTROLLER_H
#define CARGARPERSONAJECONTROLLER_H

#include <QObject>
#include <QString>
#include <qqml.h>

#include "datosConexion.h"

class CargarPersonajeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString nick READ getNick)
    Q_PROPERTY(QString password READ getPassword)
    QML_ELEMENT

public:
    explicit CargarPersonajeController(QObject* parent = nullptr);
    std::pair<DatosPersonaje, std::pair<bool, bool>> ejecutar();
public slots:
    void setNick(const QString& nick);
    void setPassword(const QString& password);
    bool esTextoValido(const QString& texto) const;
    void volverAlMenu();
    void volverACrearPersonaje();

    QString getNick() const;
    QString getPassword() const;

signals:
    void cargarPersonajeCompleted();

private:
    DatosPersonaje datosPersonaje;
    bool _volverAlMenu = false;
    bool _volverACrearPersonaje = false;
};

#endif