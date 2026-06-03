#include "editor_window.h"

#include <exception>

#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include "common/persistencia/escritor_mapa.h"
#include "common/persistencia/lector_mapa.h"

#define EDITOR_MAPA_DEFAULT "config/mapa.bin"
#define EDITOR_ANCHO_DEFAULT 100
#define EDITOR_ALTO_DEFAULT 100

EditorWindow::EditorWindow():
        modelo(EDITOR_ANCHO_DEFAULT, EDITOR_ALTO_DEFAULT), canvas(nullptr) {
    setWindowTitle("Argentum - Editor de mapas");

    canvas = new MapaCanvas(&modelo, this);
    setCentralWidget(canvas);

    crearHerramientas();
    crearMenu();
    
    intentarCargar(EDITOR_MAPA_DEFAULT);
    resize(820, 860);
}

void EditorWindow::crearHerramientas() {
    QToolBar* barra = addToolBar("Herramientas");
    QActionGroup* grupo = new QActionGroup(this);
    grupo->setExclusive(true);

    QAction* pared = barra->addAction("Pared");
    QAction* ciudad = barra->addAction("Ciudad");
    QAction* sacerdote = barra->addAction("Sacerdote");
    QAction* comerciante = barra->addAction("Comerciante");
    QAction* banquero = barra->addAction("Banquero");
    QAction* borrar = barra->addAction("Borrar");

    for (QAction* accion : {pared, ciudad, sacerdote, comerciante, banquero, borrar}) {
        accion->setCheckable(true);
        grupo->addAction(accion);
    }
    pared->setChecked(true);

    connect(pared, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaEditor::Pared); });
    connect(ciudad, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaEditor::Ciudad); });
    connect(sacerdote, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaEditor::Sacerdote); });
    connect(comerciante, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaEditor::Comerciante); });
    connect(banquero, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaEditor::Banquero); });
    connect(borrar, &QAction::triggered, this,
            [this]() { canvas->setHerramienta(HerramientaEditor::Borrar); });
}

void EditorWindow::crearMenu() {
    QMenu* archivo = menuBar()->addMenu("Archivo");

    QAction* nuevo = archivo->addAction("Nuevo");
    QAction* abrir = archivo->addAction("Abrir...");
    QAction* guardar = archivo->addAction("Guardar...");

    connect(nuevo, &QAction::triggered, this, &EditorWindow::nuevoMapa);
    connect(abrir, &QAction::triggered, this, &EditorWindow::abrirMapa);
    connect(guardar, &QAction::triggered, this, &EditorWindow::guardarMapa);
}

void EditorWindow::intentarCargar(const QString& ruta) {
    try {
        MapaCargado cargado = LectorMapa::leer(ruta.toStdString());
        modelo.cargarDesde(cargado.mapa, cargado.mapaId);
        canvas->update();
        statusBar()->showMessage("Mapa cargado: " + ruta, 4000);
    } catch (const std::exception&) {
        // Si no existe o esta corrupto, arrancamos con el mapa vacio por defecto.
    }
}

void EditorWindow::nuevoMapa() {
    modelo.cargarDesde(Mapa(EDITOR_ANCHO_DEFAULT, EDITOR_ALTO_DEFAULT), 0);
    canvas->update();
    statusBar()->showMessage("Nuevo mapa vacio", 4000);
}

void EditorWindow::abrirMapa() {
    const QString ruta = QFileDialog::getOpenFileName(
            this, "Abrir mapa", "config", "Mapas (*.bin)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        MapaCargado cargado = LectorMapa::leer(ruta.toStdString());
        modelo.cargarDesde(cargado.mapa, cargado.mapaId);
        canvas->update();
        statusBar()->showMessage("Mapa cargado: " + ruta, 4000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al abrir", e.what());
    }
}

void EditorWindow::guardarMapa() {
    const QString ruta = QFileDialog::getSaveFileName(
            this, "Guardar mapa", EDITOR_MAPA_DEFAULT, "Mapas (*.bin)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        Mapa mapa = modelo.construirMapa();
        EscritorMapa::escribir(mapa, modelo.getMapaId(), ruta.toStdString());
        statusBar()->showMessage("Mapa guardado en " + ruta, 4000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al guardar", e.what());
    }
}
