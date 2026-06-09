#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>

#include "editor_mapa.h"
#include "mapa_canvas.h"

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    EditorWindow();

private:
    EditorMapa modelo;
    MapaCanvas* canvas;

    void crearHerramientas();
    void crearMenu();
    void intentarCargar(const QString& ruta);

    void nuevoMapa();
    void abrirMapa();
    void guardarMapa();
};

#endif
