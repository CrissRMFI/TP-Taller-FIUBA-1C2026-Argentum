#include "editor_window.h"

#include <exception>

#include <QAction>
#include <QActionGroup>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>

#include "common/persistencia/escritor_mapa.h"
#include "common/persistencia/lector_mapa.h"

#define EDITOR_MAPA_DEFAULT "config/mapa.toml"
#define EDITOR_MAPA_DIR "config"
#define EDITOR_ANCHO_DEFAULT 100
#define EDITOR_ALTO_DEFAULT 100

#define VENTANA_W 771
#define VENTANA_H 719

EditorWindow::EditorWindow():
        modelo(EDITOR_ANCHO_DEFAULT, EDITOR_ALTO_DEFAULT), catalogo(),
        fondo(nullptr), canvas(nullptr), panel(nullptr),
        descripcion(nullptr), recompensa(nullptr) {
    setWindowTitle("Argentum - Editor de mapas");

    fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/mapas/es_VentanaMapa.bmp"));
    fondo->setFixedSize(VENTANA_W, VENTANA_H);
    setCentralWidget(fondo);

    // Mapa: area negra de la ventana.
    canvas = new MapaCanvas(&modelo, &catalogo, fondo);
    canvas->setGeometry(31, 92, 516, 597);

    crearPanel();
    crearMenu();

    intentarCargar(EDITOR_MAPA_DEFAULT);
    actualizarInfo();
    setFixedSize(sizeHint());
}

void EditorWindow::crearPanel() {
    
    panel = new PanelElementos(&catalogo, fondo);
    panel->setGeometry(572, 212, 180, 112);
    connect(panel, &PanelElementos::seleccionCambiada, this, &EditorWindow::actualizarInfo);

    // INFORMACION: descripcion del elemento.
    descripcion = new QLabel(fondo);
    descripcion->setGeometry(580, 360, 166, 180);
    descripcion->setWordWrap(true);
    descripcion->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    descripcion->setStyleSheet("color: rgb(206, 184, 130); background: transparent;");

    // RECOMPENSAS: oro (debajo del rotulo RECOMPENSAS).
    recompensa = new QLabel(fondo);
    recompensa->setGeometry(652, 630, 96, 18);
    recompensa->setStyleSheet("color: rgb(240, 210, 90); background: transparent;"" font-weight: bold;");

    // Boton "Crear Mapa": genera y guarda el .toml.
    QPushButton* botonCrear = new QPushButton("Crear Mapa", fondo);
    botonCrear->setGeometry(648, 652, 100, 40);
    connect(botonCrear, &QPushButton::clicked, this, &EditorWindow::crearMapa);
}

void EditorWindow::crearMenu() {
    QMenu* archivo = menuBar()->addMenu("Archivo");
    QAction* abrir = archivo->addAction("Abrir...");
    QAction* crear = archivo->addAction("Crear Mapa");
    connect(abrir, &QAction::triggered, this, &EditorWindow::abrirMapa);
    connect(crear, &QAction::triggered, this, &EditorWindow::crearMapa);

    // Herramientas de terreno
    QMenu* terreno = menuBar()->addMenu("Terreno");
    QActionGroup* grupo = new QActionGroup(this);
    grupo->setExclusive(true);
    QAction* ciudad = terreno->addAction("Ciudad (zona segura)");
    QAction* pared = terreno->addAction("Pared");
    for (QAction* a : {ciudad, pared}) {
        a->setCheckable(true);
        grupo->addAction(a);
    }
    ciudad->setChecked(true);
    connect(ciudad, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaCanvas::Ciudad); });
    connect(pared, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaCanvas::Pared); });
}

void EditorWindow::actualizarInfo() {
    ElementoCatalogo elem;
    if (panel->elementoActual(elem)) {
        descripcion->setText(elem.nombre + "\n\n" + elem.descripcion);
        recompensa->setText(elem.tieneOro ? QString("Oro: %1").arg(elem.oro) : QString());
    } else {
        descripcion->setText("(seccion en construccion)");
        recompensa->clear();
    }
}

void EditorWindow::intentarCargar(const QString& ruta) {
    try {
        LectorMapa lectorMapa;
        MapaCargado cargado = lectorMapa.leer(ruta.toStdString());
        modelo.cargarDesde(cargado.mapa, cargado.mapaId);
        canvas->update();
        statusBar()->showMessage("Mapa cargado: " + ruta, 4000);
    } catch (const std::exception&) {
        // Si no existe o esta corrupto, arrancamos con el mapa vacio (pasto).
    }
}

void EditorWindow::abrirMapa() {
    const QString ruta = QFileDialog::getOpenFileName(
            this, "Abrir mapa", EDITOR_MAPA_DIR, "Mapas (*.toml)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        LectorMapa lectorMapa;
        MapaCargado cargado = lectorMapa.leer(ruta.toStdString());
        modelo.cargarDesde(cargado.mapa, cargado.mapaId);
        canvas->update();
        statusBar()->showMessage("Mapa cargado: " + ruta, 4000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al abrir", e.what());
    }
}

QString EditorWindow::proximoArchivoMapa() const {
    
    for (int n = 2; ; ++n) {
        const QString ruta = QString("%1/mapa%2.toml").arg(EDITOR_MAPA_DIR).arg(n);
        if (!QFileInfo::exists(ruta)) {
            return ruta;
        }
    }
}

void EditorWindow::crearMapa() {
    QDir().mkpath(EDITOR_MAPA_DIR);
    const QString ruta = proximoArchivoMapa();
    try {
        Mapa mapa = modelo.construirMapa();
        EscritorMapa escritorMapa;
        escritorMapa.escribir(mapa, modelo.getMapaId(), ruta.toStdString());
        statusBar()->showMessage("Mapa creado: " + ruta, 5000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al crear el mapa", e.what());
    }
}
