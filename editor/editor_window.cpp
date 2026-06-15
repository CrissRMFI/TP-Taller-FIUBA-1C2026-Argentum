#include "editor_window.h"

#include <exception>
#include <filesystem>
#include <optional>
#include <string>

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QSpinBox>
#include <QStatusBar>

#include "common/game/mapa/mapa.h"
#include "common/persistencia/escritor_mapa.h"
#include "common/persistencia/lector_mapa.h"

#define EDITOR_MAPA_DEFAULT "config/mapa.toml"
#define EDITOR_MAPA_DIR "config"
#define EDITOR_ANCHO_DEFAULT 100
#define EDITOR_ALTO_DEFAULT 100
#define EDITOR_MAZMORRA_ANCHO 40
#define EDITOR_MAZMORRA_ALTO 40
#define EDITOR_MAZMORRA_ID 1

#define VENTANA_W 771
#define VENTANA_H 719

EditorWindow::EditorWindow(OpcionInicio opcion, const QString& ruta):
        modeloExterior(EDITOR_ANCHO_DEFAULT, EDITOR_ALTO_DEFAULT),
        modeloMazmorra(EDITOR_MAZMORRA_ANCHO, EDITOR_MAZMORRA_ALTO),
        activo(&modeloExterior), catalogo(),
        fondo(nullptr), canvas(nullptr), panel(nullptr),
        descripcion(nullptr), recompensa(nullptr), barras(nullptr),
        tabExterior(nullptr), tabMazmorra(nullptr), rutaActual(), volver(false) {
    setWindowTitle("Argentum - Editor de mapas");

    fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/mapas/es_VentanaMapa.bmp"));
    fondo->setFixedSize(VENTANA_W, VENTANA_H);
    setCentralWidget(fondo);

    // Mapa: area negra de la ventana.
    canvas = new MapaCanvas(activo, &catalogo, fondo);
    canvas->setGeometry(31, 92, 516, 597);

    crearPanel();
    crearSolapas();
    crearMenu();

    connect(canvas, &MapaCanvas::aviso, this, [this](const QString& mensaje) {
        statusBar()->showMessage(mensaje, 4000);
    });

    // Cartel de controles, siempre visible (a la derecha de la barra de estado).
    QLabel* ayuda = new QLabel(
            "Click izq: pintar zona / arrastrar elemento  ·  "
            "Click der: borrar  ·  Ctrl+rueda: zoom  ·  boton del medio: mover");
    statusBar()->addPermanentWidget(ayuda);

    // Arranque segun lo elegido en la pantalla de inicio: cargar el escenario o, si es nuevo, exterior vacio + mazmorra default.
    if (opcion == OpcionInicio::Cargar && !ruta.isEmpty()) {
        cargarEscenario(ruta);
    } else {
        mazmorraDefaultEn(modeloMazmorra, EDITOR_MAZMORRA_ID);
        sincronizarMarcadores();
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

    // Boton "Guardar": guarda el escenario sobre el archivo actual.
    QPushButton* botonGuardar = new QPushButton("Guardar", fondo);
    botonGuardar->setGeometry(648, 652, 100, 40);
    connect(botonGuardar, &QPushButton::clicked, this, &EditorWindow::guardar);
}

bool EditorWindow::pidioVolver() const { return volver; }

void EditorWindow::crearSolapas() {
    // Solapas para alternar que mapa del escenario se edita (exterior / mazmorra).
    const QString estilo =
            "QPushButton{color: rgb(206, 184, 130); background: rgba(0, 0, 0, 90);"
            " border: 1px solid rgb(120, 100, 60); padding: 2px;}"
            "QPushButton:checked{background: rgba(70, 55, 25, 200);"
            " border: 1px solid rgb(206, 184, 130); font-weight: bold;}";

    tabExterior = new QPushButton("Exterior", fondo);
    tabExterior->setGeometry(31, 66, 100, 22);
    tabExterior->setCheckable(true);
    tabExterior->setChecked(true);
    tabExterior->setStyleSheet(estilo);
    tabExterior->setCursor(Qt::PointingHandCursor);
    connect(tabExterior, &QPushButton::clicked, this, [this] { cambiarSolapa(false); });

    tabMazmorra = new QPushButton("Mazmorra", fondo);
    tabMazmorra->setGeometry(135, 66, 100, 22);
    tabMazmorra->setCheckable(true);
    tabMazmorra->setStyleSheet(estilo);
    tabMazmorra->setCursor(Qt::PointingHandCursor);
    connect(tabMazmorra, &QPushButton::clicked, this, [this] { cambiarSolapa(true); });
}

void EditorWindow::crearMenu() {
    QMenu* archivo = menuBar()->addMenu("Archivo");
    QAction* abrir = archivo->addAction("Abrir...");
    QAction* guardarComoAccion = archivo->addAction("Guardar como...");
    archivo->addSeparator();
    QAction* atras = archivo->addAction("Atras");
    connect(abrir, &QAction::triggered, this, &EditorWindow::abrirMapa);
    connect(guardarComoAccion, &QAction::triggered, this, &EditorWindow::guardarComo);
    connect(atras, &QAction::triggered, this, &EditorWindow::volverAtras);

    // "Redimensionar" es una accion directa del menu (no abre submenu).
    QAction* redimensionar = menuBar()->addAction("Redimensionar");
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

QString EditorWindow::rutaMazmorraDe(const QString& rutaExterior,
                                     const std::optional<VinculoMazmorra>& vinculo) const {
    namespace fs = std::filesystem;
    const fs::path ext(rutaExterior.toStdString());
    const std::string nombre = (vinculo.has_value() && !vinculo->archivo.empty())
                                       ? vinculo->archivo
                                       : ext.stem().string() + ".mazmorra.toml";
    return QString::fromStdString((ext.parent_path() / nombre).string());
}

QString EditorWindow::rutaExteriorDe(const QString& rutaMazmorra) const {
    namespace fs = std::filesystem;
    const fs::path maz(rutaMazmorra.toStdString());
    const std::string nombre = maz.filename().string();
    const std::string sufijo = ".mazmorra.toml";
    std::string base;
    if (nombre.size() > sufijo.size() &&
        nombre.compare(nombre.size() - sufijo.size(), sufijo.size(), sufijo) == 0) {
        base = nombre.substr(0, nombre.size() - sufijo.size());
    } else {
        base = maz.stem().string();
    }
    return QString::fromStdString((maz.parent_path() / (base + ".toml")).string());
}

void EditorWindow::mazmorraDefaultEn(EditorMapa& modelo, uint16_t id) {
    // Mazmorra default: solo piso base de caverna; sin monstruos, paredes ni objetos.
    Mapa m(EDITOR_MAZMORRA_ANCHO, EDITOR_MAZMORRA_ALTO);
    m.agregarPiso(ZonaPiso{id, 0, 0,
                           static_cast<uint16_t>(EDITOR_MAZMORRA_ANCHO - 1),
                           static_cast<uint16_t>(EDITOR_MAZMORRA_ALTO - 1), "piedra_oscura"});
    modelo.cargarDesde(m, id, std::nullopt);
}

void EditorWindow::exteriorDefaultEn(EditorMapa& modelo) {
    // Exterior default: vacio (sin zonas -> piso base pasto, transitable).
    Mapa m(EDITOR_ANCHO_DEFAULT, EDITOR_ALTO_DEFAULT);
    modelo.cargarDesde(m, 0, std::nullopt);
}

void EditorWindow::cargarEnModeloODefault(EditorMapa& modelo, const QString& path,
                                          bool comoMazmorra) {
    try {
        LectorMapa lector;
        MapaCargado cargado = lector.leer(path.toStdString());
        modelo.cargarDesde(cargado.mapa, cargado.mapaId, cargado.vinculoMazmorra);
    } catch (const std::exception&) {
        // No existe o esta corrupto: generamos un default para no quedarnos sin par.
        if (comoMazmorra) {
            mazmorraDefaultEn(modelo, EDITOR_MAZMORRA_ID);
        } else {
            exteriorDefaultEn(modelo);
        }
    }
}

void EditorWindow::cargarEscenario(const QString& rutaElegida) {
    try {
        LectorMapa lector;
        MapaCargado elegido = lector.leer(rutaElegida.toStdString());
        if (elegido.mapaId == 0) {
            // Abrieron el exterior: cargamos su mazmorra vinculada (o una default).
            const QString pathMazmorra = rutaMazmorraDe(rutaElegida, elegido.vinculoMazmorra);
            modeloExterior.cargarDesde(elegido.mapa, elegido.mapaId, elegido.vinculoMazmorra);
            cargarEnModeloODefault(modeloMazmorra, pathMazmorra, /*comoMazmorra=*/true);
            rutaActual = rutaElegida;
        } else {
            // Abrieron la mazmorra: cargamos su exterior (por convencion de nombre).
            const QString pathExterior = rutaExteriorDe(rutaElegida);
            modeloMazmorra.cargarDesde(elegido.mapa, elegido.mapaId, elegido.vinculoMazmorra);
            cargarEnModeloODefault(modeloExterior, pathExterior, /*comoMazmorra=*/false);
            rutaActual = pathExterior;
        }
        sincronizarMarcadores();
        cambiarSolapa(false);  // arrancamos en la solapa Exterior
        statusBar()->showMessage("Escenario cargado: " + rutaActual, 4000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al abrir", e.what());
    }
}

void EditorWindow::cambiarSolapa(bool aMazmorra) {
    activo = aMazmorra ? &modeloMazmorra : &modeloExterior;
    if (tabExterior != nullptr) {
        tabExterior->setChecked(!aMazmorra);
    }
    if (tabMazmorra != nullptr) {
        tabMazmorra->setChecked(aMazmorra);
    }
    canvas->setModelo(activo);  // reencuadra solo
    actualizarInfo();
}

void EditorWindow::sincronizarMarcadores() {
    const std::optional<VinculoMazmorra>& v = modeloExterior.getVinculoMazmorra();
    if (v.has_value()) {
        modeloExterior.setMarcador(v->entradaX, v->entradaY);
        modeloMazmorra.setMarcador(v->salidaX, v->salidaY);
    } else {
        modeloExterior.setMarcador(2, 2);
        modeloMazmorra.setMarcador(1, 1);
    }
}

void EditorWindow::abrirMapa() {
    const QString ruta = QFileDialog::getOpenFileName(
            this, "Abrir mapa", EDITOR_MAPA_DIR, "Mapas (*.toml)");
    if (ruta.isEmpty()) {
        return;
    }
    cargarEscenario(ruta);
}

void EditorWindow::guardarEscenario(const QString& rutaExteriorDestino) {
    namespace fs = std::filesystem;
    const fs::path destino(rutaExteriorDestino.toStdString());
    const std::string mazmorraNombre = destino.stem().string() + ".mazmorra.toml";
    const fs::path mazmorraDestino = destino.parent_path() / mazmorraNombre;

    // Vinculo del exterior: preservamos los portales cargados o usamos defaults.
    VinculoMazmorra vinculo;
    if (modeloExterior.getVinculoMazmorra().has_value()) {
        vinculo = *modeloExterior.getVinculoMazmorra();
    } else {
        vinculo.entradaX = 2; vinculo.entradaY = 2;
        vinculo.entradaDestinoX = 2; vinculo.entradaDestinoY = 2;
        vinculo.salidaX = 1; vinculo.salidaY = 1;
        vinculo.salidaDestinoX = 3; vinculo.salidaDestinoY = 2;
    }
    vinculo.archivo = mazmorraNombre;
    vinculo.entradaX = modeloExterior.getMarcadorX();
    vinculo.entradaY = modeloExterior.getMarcadorY();
    vinculo.salidaX = modeloMazmorra.getMarcadorX();
    vinculo.salidaY = modeloMazmorra.getMarcadorY();

    EscritorMapa escritor;
    escritor.escribir(modeloExterior.construirMapa(), modeloExterior.getMapaId(),
                      rutaExteriorDestino.toStdString(), &vinculo);
    escritor.escribir(modeloMazmorra.construirMapa(), modeloMazmorra.getMapaId(),
                      mazmorraDestino.string());
}

void EditorWindow::guardar() {
    // Guarda sobre el archivo actual. Si el escenario es nuevo (nunca se guardo),
    // se comporta como "Guardar como" para que el usuario elija la ruta.
    if (rutaActual.isEmpty()) {
        guardarComo();
        return;
    }
    QDir().mkpath(EDITOR_MAPA_DIR);
    try {
        guardarEscenario(rutaActual);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al guardar el mapa", e.what());
        return;
    }
    statusBar()->showMessage("Guardado: " + rutaActual + " (+ mazmorra)", 4000);
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
        guardarEscenario(ruta);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error al guardar el mapa", e.what());
        return;
    }
    rutaActual = ruta;  // a partir de ahora "Guardar" usa esta ruta
    statusBar()->showMessage("Escenario guardado: " + ruta + " (+ mazmorra)", 4000);
}

void EditorWindow::volverAtras() {
    const auto r = QMessageBox::question(
            this, "Volver al inicio",
            "Vas a volver a la pantalla de inicio y se perdera el progreso no "
            "guardado. Continuar?");
    if (r != QMessageBox::Yes) {
        return;
    }
    volver = true;
    close();
}

void EditorWindow::redimensionarMapa() {
    // Un solo dialogo con ancho y alto.
    QDialog dialogo(this);
    dialogo.setWindowTitle("Redimensionar mapa");
    QFormLayout* form = new QFormLayout(&dialogo);

    QSpinBox* spinAncho = new QSpinBox(&dialogo);
    spinAncho->setRange(1, 1000);
    spinAncho->setValue(activo->getAncho());
    QSpinBox* spinAlto = new QSpinBox(&dialogo);
    spinAlto->setRange(1, 1000);
    spinAlto->setValue(activo->getAlto());
    form->addRow("Ancho (celdas):", spinAncho);
    form->addRow("Alto (celdas):", spinAlto);

    QDialogButtonBox* botones = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialogo);
    form->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }
    const int nuevoAncho = spinAncho->value();
    const int nuevoAlto = spinAlto->value();

    // Si achica, avisamos: se descarta lo que quede fuera del nuevo rectangulo.
    if (nuevoAncho < activo->getAncho() || nuevoAlto < activo->getAlto()) {
        const auto r = QMessageBox::question(
                this, "Achicar mapa",
                "Al achicar el mapa se descarta el contenido que quede fuera del "
                "nuevo tamano. Continuar?");
        if (r != QMessageBox::Yes) {
            return;
        }
    }

    activo->redimensionar(static_cast<uint16_t>(nuevoAncho), static_cast<uint16_t>(nuevoAlto));
    canvas->reencuadrar();
    statusBar()->showMessage(
            QString("Mapa redimensionado a %1 x %2").arg(nuevoAncho).arg(nuevoAlto), 4000);
}
