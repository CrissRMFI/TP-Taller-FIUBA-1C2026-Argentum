#ifndef DIALOGO_NOMBRE_MAPA_H
#define DIALOGO_NOMBRE_MAPA_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

// Ventana "CONFIRMAR" (usa es_ventanaconfirmar.bmp de fondo) que pide el nombre
// del mapa antes de crearlo. ACEPTAR confirma (accept), CANCELAR / X cancela.
class DialogoNombreMapa : public QDialog {
    Q_OBJECT

public:
    explicit DialogoNombreMapa(QWidget* parent = nullptr);

    QString nombre() const;

private:
    QLineEdit* entrada;

    void intentarAceptar();
};

#endif
