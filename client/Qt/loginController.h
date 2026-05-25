#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

#include <QObject>
#include <QString>
#include <qqml.h>

#include "datosConexion.h"

class LoginController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString puerto READ getPuerto)
    Q_PROPERTY(QString host READ getHost)
    QML_ELEMENT
public:
    explicit LoginController(QObject* parent = nullptr);
    DatosLogin ejecutar();

public slots:
    void setPuerto(const QString& puerto);
    void setHost(const QString& host);

    bool esPuertoValido(const QString& puerto) const;
    bool esHostValido(const QString& host) const;

    QString getPuerto() const;
    QString getHost() const;

signals:
    void loginCompleted();

private:
QString selectedPuerto;
QString selectedHost;

};

#endif