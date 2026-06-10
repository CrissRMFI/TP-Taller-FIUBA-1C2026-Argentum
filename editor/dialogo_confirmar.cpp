#include "dialogo_confirmar.h"

#include <QLabel>
#include <QPixmap>
#include <QPushButton>

#define CONFIRMAR_W 314
#define CONFIRMAR_H 213

DialogoConfirmar::DialogoConfirmar(QWidget* parent): QDialog(parent) {
    setWindowTitle("Confirmar");
    setFixedSize(CONFIRMAR_W, CONFIRMAR_H);

    // Fondo de la ventana
    QLabel* fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/mapas/es_ventanaconfirmar.bmp"));
    fondo->setGeometry(0, 0, CONFIRMAR_W, CONFIRMAR_H);

    // Mensaje centrado en el area de contenido.
    QLabel* mensaje = new QLabel("¿Sobreescribir el mapa actual?", fondo);
    mensaje->setGeometry(20, 95, 274, 24);
    mensaje->setAlignment(Qt::AlignCenter);
    mensaje->setStyleSheet("color: rgb(206, 184, 130); background: transparent;");

    // Botones (CANCELAR / ACEPTAR / X).
    const QString flat = "QPushButton{background: transparent; border: none;}";

    QPushButton* cancelar = new QPushButton(fondo);
    cancelar->setGeometry(32, 167, 118, 28);
    cancelar->setStyleSheet(flat);
    cancelar->setCursor(Qt::PointingHandCursor);
    connect(cancelar, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton* aceptar = new QPushButton(fondo);
    aceptar->setGeometry(165, 167, 120, 28);
    aceptar->setStyleSheet(flat);
    aceptar->setCursor(Qt::PointingHandCursor);
    connect(aceptar, &QPushButton::clicked, this, &QDialog::accept);

    QPushButton* cerrar = new QPushButton(fondo);
    cerrar->setGeometry(288, 6, 22, 22);
    cerrar->setStyleSheet(flat);
    cerrar->setCursor(Qt::PointingHandCursor);
    connect(cerrar, &QPushButton::clicked, this, &QDialog::reject);
}
