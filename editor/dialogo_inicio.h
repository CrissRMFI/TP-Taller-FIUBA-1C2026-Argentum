#ifndef DIALOGO_INICIO_H
#define DIALOGO_INICIO_H

#include <QDialog>
#include <QString>


enum class OpcionInicio { Cancelar, Cargar, Crear };


class DialogoInicio : public QDialog {
    Q_OBJECT

public:
    explicit DialogoInicio(QWidget* parent = nullptr);

    OpcionInicio opcion() const;
    QString rutaElegida() const;

private:
    OpcionInicio eleccion;
    QString ruta;

    void elegirCargar();
    void elegirCrear();
};

#endif
