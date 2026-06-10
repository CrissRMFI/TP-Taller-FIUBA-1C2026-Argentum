#include "editor_window.h"

#include <exception>

#include <algorithm>

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>

#include <toml++/toml.hpp>

#include "common/persistencia/escritor_mapa.h"
#include "common/persistencia/lector_mapa.h"
#include "dialogo_nombre_mapa.h"

#define EDITOR_MAPA_DEFAULT "config/mapa.toml"
#define EDITOR_MAPA_DIR "config"
#define EDITOR_ANCHO_DEFAULT 100
#define EDITOR_ALTO_DEFAULT 100

#define VENTANA_W 771
#define VENTANA_H 719

EditorWindow::EditorWindow():
        modelo(EDITOR_ANCHO_DEFAULT, EDITOR_ALTO_DEFAULT), catalogo(),
        fondo(nullptr), canvas(nullptr), panel(nullptr),
        descripcion(nullptr), recompensa(nullptr), barras(nullptr) {
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

    // Cartel de controles, siempre visible (a la derecha de la barra de estado).
    QLabel* ayuda = new QLabel(
            "Click izq: pintar zona / arrastrar elemento  ·  "
            "Click der: borrar  ·  Ctrl+rueda: zoom  ·  boton del medio: mover");
    statusBar()->addPermanentWidget(ayuda);

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
    descripcion->setStyleSheet(
            "color: rgb(206, 184, 130); background: transparent; font-size: 11px;");

    // RECOMPENSAS: oro (debajo del rotulo RECOMPENSAS).
    recompensa = new QLabel(fondo);
    recompensa->setGeometry(652, 630, 96, 18);
    recompensa->setStyleSheet("color: rgb(240, 210, 90); background: transparent;"" font-weight: bold;");

    // Barras de stats (vida / danio / nivel / oro) sobre los slots del bmp.
    barras = new BarrasStats(&catalogo, fondo);
    barras->setGeometry(600, 555, 156, 42);

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
}

void EditorWindow::actualizarInfo() {
    ElementoCatalogo elem;
    const bool hayElem = panel->elementoActual(elem);
    if (hayElem) {
        descripcion->setText(elem.nombre + "\n\n" + elem.descripcion);
        recompensa->setText(elem.tieneOro ? QString("Oro: %1").arg(elem.oro) : QString());
        barras->setStats(elem);
    } else {
        descripcion->setText("(seccion en construccion)");
        recompensa->clear();
        barras->limpiar();
    }

    
    if (hayElem && panel->seccionActual() == SeccionCatalogo::Pisos) {
        canvas->setPincelPiso(true, elem.clave, elem.destino);
    } else {
        canvas->setPincelPiso(false, QString(), QString());
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

uint16_t EditorWindow::proximoMapaId() const {
    // mapa_id unico = max de los mapa_id existentes en config/*.toml + 1.
    int maxId = -1;
    const QDir dir(EDITOR_MAPA_DIR);
    for (const QString& f : dir.entryList(QStringList() << "*.toml", QDir::Files)) {
        try {
            const toml::table tbl =
                    toml::parse_file((QString(EDITOR_MAPA_DIR) + "/" + f).toStdString());
            if (const auto id = tbl["mapa_id"].value<int64_t>()) {
                maxId = std::max(maxId, static_cast<int>(*id));
            }
        } catch (...) {
            // No es un mapa valido (criaturas.toml, pisos.toml, ...): se ignora.
        }
    }
    return static_cast<uint16_t>(maxId + 1);
}

void EditorWindow::crearMapa() {
    if (!modelo.todoCubierto()) {
        QMessageBox::warning(this, "Falta piso",
                             "No se puede crear el mapa: hay celdas sin piso. "
                             "Pinta el terreno hasta cubrir todo el mapa.");
        return;
    }

    // Ventana CONFIRMAR: pide el nombre. Cancelar => seguir editando.
    DialogoNombreMapa dialogo(this);
    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }

    // Nombre -> archivo seguro (config/<nombre>.toml).
    QString base;
    for (const QChar& c : dialogo.nombre()) {
        if (c.isLetterOrNumber() || c == '-' || c == '_') {
            base += c;
        } else if (c == ' ') {
            base += '_';
        }
    }
    if (base.isEmpty()) {
        base = "mapa";
    }
    QDir().mkpath(EDITOR_MAPA_DIR);
    const QString ruta = QString("%1/%2.toml").arg(EDITOR_MAPA_DIR, base);

    // Id unico para el mapa nuevo (base para multi-mapa a futuro).
    modelo.setMapaId(proximoMapaId());
    try {
        Mapa mapa = modelo.construirMapa();
        EscritorMapa escritorMapa;
        escritorMapa.escribir(mapa, modelo.getMapaId(), ruta.toStdString());
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al crear el mapa", e.what());
        return;  // si fallo el guardado, no cerramos el editor
    }

    // Confirmado y guardado: se cierra el editor.
    qApp->quit();
}
