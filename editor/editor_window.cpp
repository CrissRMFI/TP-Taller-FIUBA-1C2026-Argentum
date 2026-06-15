#include "editor_window.h"

#include <exception>

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>

#include "common/persistencia/escritor_mapa.h"
#include "common/persistencia/lector_mapa.h"
#include "dialogo_confirmar.h"

#define EDITOR_MAPA_DEFAULT "config/mapa.toml"
#define EDITOR_MAPA_DIR "config"
#define EDITOR_ANCHO_DEFAULT 100
#define EDITOR_ALTO_DEFAULT 100

#define VENTANA_W 771
#define VENTANA_H 719

EditorWindow::EditorWindow(OpcionInicio opcion, const QString& ruta):
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

    
    connect(canvas, &MapaCanvas::aviso, this, [this](const QString& mensaje) {
        statusBar()->showMessage(mensaje, 4000);
    });

    // Cartel de controles, siempre visible (a la derecha de la barra de estado).
    QLabel* ayuda = new QLabel(
            "Click izq: pintar zona / arrastrar elemento  ·  "
            "Click der: borrar  ·  Ctrl+rueda: zoom  ·  boton del medio: mover");
    statusBar()->addPermanentWidget(ayuda);

    // Arranque segun lo elegido en la pantalla de inicio
    if (opcion == OpcionInicio::Cargar && !ruta.isEmpty()) {
        intentarCargar(ruta);
    }
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
    QAction* guardarComoAccion = archivo->addAction("Guardar como...");
    QAction* crear = archivo->addAction("Crear Mapa");
    connect(abrir, &QAction::triggered, this, &EditorWindow::abrirMapa);
    connect(guardarComoAccion, &QAction::triggered, this, &EditorWindow::guardarComo);
    connect(crear, &QAction::triggered, this, &EditorWindow::crearMapa);

    QMenu* mapa = menuBar()->addMenu("Mapa");
    QAction* redimensionar = mapa->addAction("Redimensionar...");
    connect(redimensionar, &QAction::triggered, this, &EditorWindow::redimensionarMapa);
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

void EditorWindow::guardarComo() {
    QString ruta = QFileDialog::getSaveFileName(
            this, "Guardar mapa como", EDITOR_MAPA_DIR, "Mapas (*.toml)");
    if (ruta.isEmpty()) {
        return;
    }
    if (!ruta.endsWith(".toml", Qt::CaseInsensitive)) {
        ruta += ".toml";
    }
    try {
        Mapa mapa = modelo.construirMapa();
        EscritorMapa escritorMapa;
        escritorMapa.escribir(mapa, modelo.getMapaId(), ruta.toStdString());
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al guardar el mapa", e.what());
        return;
    }
    statusBar()->showMessage("Mapa guardado en: " + ruta, 4000);
}

void EditorWindow::redimensionarMapa() {
    bool ok = false;
    const int nuevoAncho = QInputDialog::getInt(
            this, "Redimensionar mapa", "Ancho (celdas):", modelo.getAncho(), 1, 1000, 1, &ok);
    if (!ok) {
        return;
    }
    const int nuevoAlto = QInputDialog::getInt(
            this, "Redimensionar mapa", "Alto (celdas):", modelo.getAlto(), 1, 1000, 1, &ok);
    if (!ok) {
        return;
    }

    // Si achica, avisamos: se descarta lo que quede fuera del nuevo rectangulo.
    if (nuevoAncho < modelo.getAncho() || nuevoAlto < modelo.getAlto()) {
        const auto r = QMessageBox::question(
                this, "Achicar mapa",
                "Al achicar el mapa se descarta el contenido que quede fuera del "
                "nuevo tamano. Continuar?");
        if (r != QMessageBox::Yes) {
            return;
        }
    }

    modelo.redimensionar(static_cast<uint16_t>(nuevoAncho), static_cast<uint16_t>(nuevoAlto));
    canvas->reencuadrar();
    statusBar()->showMessage(
            QString("Mapa redimensionado a %1 x %2").arg(nuevoAncho).arg(nuevoAlto), 4000);
}

void EditorWindow::crearMapa() {
    if (!modelo.todoCubierto()) {
        QMessageBox::warning(this, "Falta piso",
                             "No se puede crear el mapa: hay celdas sin piso. "
                             "Pinta el terreno hasta cubrir todo el mapa.");
        return;
    }

    // Ventana CONFIRMAR: sobreescribe el mapa del server. Cancelar => seguir editando.
    DialogoConfirmar dialogo(this);
    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }

    QDir().mkpath(EDITOR_MAPA_DIR);
    try {
        Mapa mapa = modelo.construirMapa();
        EscritorMapa escritorMapa;
        escritorMapa.escribir(mapa, modelo.getMapaId(), EDITOR_MAPA_DEFAULT);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al guardar el mapa", e.what());
        return;
    }


    qApp->quit();
}
