#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

#include <qqml.h>
#include <QObject>
#include <QString>

#include "datosConexion.h"

class QQuickView;

class LoginController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString puerto READ getPuerto)
    Q_PROPERTY(QString host READ getHost)
    QML_ELEMENT
public:
    explicit LoginController(QObject* parent = nullptr);
    void run(QQuickView& ventana, DatosConexion& datos);

public slots:
    void setPuerto(const QString& puerto);
    void setHost(const QString& host);

    bool esPuertoValido(const QString& puerto) const;
    bool esHostIpValido(const QString& hostIp) const;
    bool huboErrorLogin() const;

    QString getPuerto() const;
    QString getHost() const;
    QString getErrorLogin() const;

signals:
    void loginCompleted();

private:
    QString selectedPuerto;
    QString selectedHost;
    bool errorLogin = false;
    QString errorLoginMessage;
};

#endif