#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QLabel>
#include <QMainWindow>

#include "barras_stats.h"
#include "catalogo_editor.h"
#include "editor_mapa.h"
#include "mapa_canvas.h"
#include "panel_elementos.h"

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    EditorWindow();

private:
    EditorMapa modelo;
    CatalogoEditor catalogo;
    QLabel* fondo;
    MapaCanvas* canvas;
    PanelElementos* panel;
    QLabel* descripcion;
    QLabel* recompensa;
    BarrasStats* barras;

    void crearPanel();
    void crearMenu();
    void intentarCargar(const QString& ruta);

    void abrirMapa();
    void crearMapa();
    void actualizarInfo();
};

#endif
