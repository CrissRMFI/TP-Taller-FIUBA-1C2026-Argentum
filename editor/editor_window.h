#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <optional>

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QString>

#include "barras_stats.h"
#include "catalogo_editor.h"
#include "dialogo_inicio.h"
#include "editor_mapa.h"
#include "mapa_canvas.h"
#include "panel_elementos.h"

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    EditorWindow(OpcionInicio opcion, const QString& ruta);

    // true si el usuario pidio "Atras" (volver a la pantalla de inicio).
    bool pidioVolver() const;

private:
    EditorMapa modeloExterior;
    EditorMapa modeloMazmorra;
    EditorMapa* activo;
    CatalogoEditor catalogo;
    QLabel* fondo;
    MapaCanvas* canvas;
    PanelElementos* panel;
    QLabel* descripcion;
    QLabel* recompensa;
    BarrasStats* barras;
    QPushButton* tabExterior;
    QPushButton* tabMazmorra;
    QString rutaActual;
    bool volver;

    void crearPanel();
    void crearMenu();
    void crearSolapas();

    // Carga el escenario (exterior + su mazmorra) a partir de cualquiera de los dos archivos
    void cargarEscenario(const QString& rutaElegida);
    void cargarEnModeloODefault(EditorMapa& modelo, const QString& path, bool comoMazmorra);
    void mazmorraDefaultEn(EditorMapa& modelo, uint16_t id);
    void exteriorDefaultEn(EditorMapa& modelo);
    void cambiarSolapa(bool aMazmorra);
    void sincronizarMarcadores();

    void guardarEscenario(const QString& rutaExteriorDestino);

    QString rutaMazmorraDe(const QString& rutaExterior,
                           const std::optional<VinculoMazmorra>& vinculo) const;
    QString rutaExteriorDe(const QString& rutaMazmorra) const;

    void abrirMapa();
    void guardar(); // guarda el escenario sobre el archivo actual
    void guardarComo(); // guarda el escenario eligiendo ruta
    void volverAtras(); // vuelve a la pantalla de inicio (con confirmacion)
    void redimensionarMapa();
    void actualizarInfo();
};

#endif
