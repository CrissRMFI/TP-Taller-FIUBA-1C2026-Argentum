#include "dialogo_nombre_mapa.h"

#include <QLabel>
#include <QPixmap>
#include <QPushButton>

#define CONFIRMAR_W 314
#define CONFIRMAR_H 213

DialogoNombreMapa::DialogoNombreMapa(QWidget* parent):
        QDialog(parent), entrada(nullptr) {
    setWindowTitle("Crear mapa");
    setFixedSize(CONFIRMAR_W, CONFIRMAR_H);

   
    QLabel* fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/mapas/es_ventanaconfirmar.bmp"));
    fondo->setGeometry(0, 0, CONFIRMAR_W, CONFIRMAR_H);

    
    QLabel* titulo = new QLabel("Nombre del mapa:", fondo);
    titulo->setGeometry(40, 80, 234, 18);
    titulo->setStyleSheet("color: rgb(206, 184, 130); background: transparent;");

    entrada = new QLineEdit(fondo);
    entrada->setGeometry(57, 104, 200, 26);
    entrada->setAlignment(Qt::AlignCenter);
    entrada->setMaxLength(40);

    
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
    connect(aceptar, &QPushButton::clicked, this, &DialogoNombreMapa::intentarAceptar);

    QPushButton* cerrar = new QPushButton(fondo);
    cerrar->setGeometry(288, 6, 22, 22);
    cerrar->setStyleSheet(flat);
    cerrar->setCursor(Qt::PointingHandCursor);
    connect(cerrar, &QPushButton::clicked, this, &QDialog::reject);

    // Enter en el campo confirma.
    connect(entrada, &QLineEdit::returnPressed, this, &DialogoNombreMapa::intentarAceptar);
    entrada->setFocus();
}

QString DialogoNombreMapa::nombre() const {
    return entrada->text().trimmed();
}

void DialogoNombreMapa::intentarAceptar() {
    if (!nombre().isEmpty()) {
        accept();
    }
}
