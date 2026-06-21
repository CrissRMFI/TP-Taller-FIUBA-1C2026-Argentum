#ifndef ELEGIR_PERSONAJE_CONTROLLER_H
#define ELEGIR_PERSONAJE_CONTROLLER_H

#include <qqml.h>
#include <QObject>
#include <QString>

#include "datosConexion.h"
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

enum class ElegirPersonajeResultado {
    VolverAlMenu,
    FinalizarRegistro,
};

class QQuickView;

class ElegirPersonajeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Raza raza READ getRaza)
    Q_PROPERTY(ClasePersonaje clase READ getClase)
    Q_PROPERTY(QString nick READ getNick)
    QML_ELEMENT
public:
    explicit ElegirPersonajeController(QObject* parent = nullptr);
    void run(QQuickView& ventana, DatosConexion& datos, ElegirPersonajeResultado& resultado);

public slots:
    void setRaza(const QString& raza);
    void setClase(const QString& clase);
    void setNick(const QString& nick);
    void setCabeza(int cabezaIndex);
    void setCuerpo(int cuerpoIndex);
    void volverAlMenu();
    bool esNickValido(const QString& texto) const;
    QString rutaCabezaPreview(int cabezaIndex) const;
    QString rutaCuerpoPreview(int cuerpoIndex) const;

    Raza getRaza() const;
    ClasePersonaje getClase() const;
    QString getNick() const;

signals:
    void elegirPersonajeCompleted();

private:
    Raza selectedRaza;
    ClasePersonaje selectedClase;
    QString selectedNick;
    int selectedCabeza = 2000;
    int selectedCuerpo = 2100;
    bool razaSeleccionada = false;
    bool claseSeleccionada = false;
    bool _volverAlMenu = false;
    bool cabezaSeleccionada = false;
    bool cuerpoSeleccionado = false;

    bool camposCompletos() const;
};

#endif
