#include "dialogo_inicio.h"

#include <exception>

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>

#include "common/persistencia/lector_mapa.h"

// Tamano de la ventana de inicio
#define INICIO_W 538
#define INICIO_H 463

#define INICIO_MAPA_DIR "config"

DialogoInicio::DialogoInicio(QWidget* parent) :
        QDialog(parent), eleccion(OpcionInicio::Cancelar), ruta() {
    setWindowTitle("Argentum - Editor de mapas");
    setFixedSize(INICIO_W, INICIO_H);

    QLabel* fondo = new QLabel(this);
    fondo->setPixmap(
            QPixmap(":/editor/marco.bmp")
                    .scaled(INICIO_W, INICIO_H, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondo->setGeometry(0, 0, INICIO_W, INICIO_H);

    QLabel* titulo = new QLabel("Editor de Mapas", fondo);
    titulo->setGeometry(38, 65, 462, 60);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet(
            "color: rgb(206, 184, 130); background: transparent;"
            " font-size: 34px; font-weight: bold;");

    const QString estiloBoton =
            "QPushButton{color: rgb(206, 184, 130);"
            " background: rgba(0, 0, 0, 90); font-size: 18px;"
            " border: 1px solid rgb(120, 100, 60); padding: 8px;}"
            "QPushButton:hover{background: rgba(70, 55, 25, 160);"
            " border: 1px solid rgb(206, 184, 130);}";

    QPushButton* cargar = new QPushButton("Cargar mapa...", fondo);
    cargar->setGeometry(113, 195, 313, 60);
    cargar->setStyleSheet(estiloBoton);
    cargar->setCursor(Qt::PointingHandCursor);
    connect(cargar, &QPushButton::clicked, this, &DialogoInicio::elegirCargar);

    QPushButton* crear = new QPushButton("Crear mapa nuevo", fondo);
    crear->setGeometry(113, 290, 313, 60);
    crear->setStyleSheet(estiloBoton);
    crear->setCursor(Qt::PointingHandCursor);
    connect(crear, &QPushButton::clicked, this, &DialogoInicio::elegirCrear);
}

OpcionInicio DialogoInicio::opcion() const {
    return eleccion;
}
QString DialogoInicio::rutaElegida() const {
    return ruta;
}

void DialogoInicio::elegirCargar() {
    const QString elegida =
            QFileDialog::getOpenFileName(this, "Abrir mapa", INICIO_MAPA_DIR, "Mapas (*.toml)");
    if (elegida.isEmpty()) {
        return;
    }
    // Solo aceptamos archivos con la firma del formato; si no, avisamos y dejamos
    // que el usuario elija otro
    try {
        LectorMapa lectorMapa;
        lectorMapa.validarFirma(elegida.toStdString());
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Archivo invalido", e.what());
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
