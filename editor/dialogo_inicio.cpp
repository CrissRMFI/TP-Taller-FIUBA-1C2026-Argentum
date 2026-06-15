#include "dialogo_inicio.h"

#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>

#define INICIO_W 215
#define INICIO_H 185

#define INICIO_MAPA_DIR "config"

DialogoInicio::DialogoInicio(QWidget* parent):
        QDialog(parent), eleccion(OpcionInicio::Cancelar), ruta() {
    setWindowTitle("Argentum - Editor de mapas");
    setFixedSize(INICIO_W, INICIO_H);

    QLabel* fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/editor/marco.bmp"));
    fondo->setGeometry(0, 0, INICIO_W, INICIO_H);

    QLabel* titulo = new QLabel("Editor de Mapas", fondo);
    titulo->setGeometry(15, 26, 185, 24);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet(
            "color: rgb(206, 184, 130); background: transparent;"
            " font-size: 15px; font-weight: bold;");

    const QString estiloBoton =
            "QPushButton{color: rgb(206, 184, 130);"
            " background: rgba(0, 0, 0, 90);"
            " border: 1px solid rgb(120, 100, 60); padding: 4px;}"
            "QPushButton:hover{background: rgba(70, 55, 25, 160);"
            " border: 1px solid rgb(206, 184, 130);}";

    QPushButton* cargar = new QPushButton("Cargar mapa...", fondo);
    cargar->setGeometry(45, 78, 125, 32);
    cargar->setStyleSheet(estiloBoton);
    cargar->setCursor(Qt::PointingHandCursor);
    connect(cargar, &QPushButton::clicked, this, &DialogoInicio::elegirCargar);

    QPushButton* crear = new QPushButton("Crear mapa nuevo", fondo);
    crear->setGeometry(45, 120, 125, 32);
    crear->setStyleSheet(estiloBoton);
    crear->setCursor(Qt::PointingHandCursor);
    connect(crear, &QPushButton::clicked, this, &DialogoInicio::elegirCrear);
}

OpcionInicio DialogoInicio::opcion() const { return eleccion; }
QString DialogoInicio::rutaElegida() const { return ruta; }

void DialogoInicio::elegirCargar() {
    const QString elegida = QFileDialog::getOpenFileName(
            this, "Abrir mapa", INICIO_MAPA_DIR, "Mapas (*.toml)");
    if (elegida.isEmpty()) {
        // Cancelo el file picker: se queda en la pantalla de inicio.
        return;
    }
    eleccion = OpcionInicio::Cargar;
    ruta = elegida;
    accept();
}

void DialogoInicio::elegirCrear() {
    eleccion = OpcionInicio::Crear;
    ruta.clear();
    accept();
}
