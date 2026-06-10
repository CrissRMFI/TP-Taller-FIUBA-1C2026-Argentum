#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QLabel>
#include <QMainWindow>

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

    void crearPanel();
    void crearMenu();
    void intentarCargar(const QString& ruta);

    void abrirMapa();
    void crearMapa();
    void actualizarInfo();
    QString proximoArchivoMapa() const;
};

#endif
