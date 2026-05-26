#ifndef ELEGIR_PERSONAJE_CONTROLLER_H
#define ELEGIR_PERSONAJE_CONTROLLER_H

#include <QObject>
#include <QString>
#include <qqml.h>

#include "datosConexion.h"
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

enum class ElegirPersonajeResultado {
    VolverAlMenu,
    FinalizarRegistro,
};

class ElegirPersonajeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Raza raza READ getRaza)
    Q_PROPERTY(ClasePersonaje clase READ getClase)
    Q_PROPERTY(QString nick READ getNick)
    Q_PROPERTY(QString password READ getPassword)
    QML_ELEMENT
public:
    explicit ElegirPersonajeController(QObject* parent = nullptr);
    void run(DatosConexion& datos, ElegirPersonajeResultado& resultado);

public slots:
    void setRaza(const QString& raza);
    void setClase(const QString& clase);
    void setNick(const QString& nick);
    void setPassword(const QString& password);
    void volverAlMenu();
    bool esTextoValido(const QString& texto) const;

    Raza getRaza() const;
    ClasePersonaje getClase() const;
    QString getNick() const;
    QString getPassword() const;

signals:
    void elegirPersonajeCompleted();

private:
    Raza selectedRaza;
    ClasePersonaje selectedClase;
    QString selectedNick;
    QString selectedPassword;
    bool razaSeleccionada = false;
    bool claseSeleccionada = false;
    bool _volverAlMenu = false;
};

#endif