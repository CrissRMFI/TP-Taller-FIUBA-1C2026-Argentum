#ifndef MAPA_CANVAS_H
#define MAPA_CANVAS_H

#include <QPoint>
#include <QWidget>

#include "editor_mapa.h"

enum class HerramientaEditor {
    Pared,
    Ciudad,
    Sacerdote,
    Comerciante,
    Banquero,
    Borrar,
};

class MapaCanvas : public QWidget {
    Q_OBJECT

public:
    explicit MapaCanvas(EditorMapa* modelo, QWidget* parent = nullptr);

    void setHerramienta(HerramientaEditor herramienta);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    EditorMapa* modelo;
    HerramientaEditor herramienta;

    // Estado del arrastre para dibujar una ciudad (rectangulo). Las coordenadas
    // son de celda; solo valen mientras dibujandoCiudad es true.
    bool dibujandoCiudad;
    uint16_t ciudadInicioX;
    uint16_t ciudadInicioY;
    uint16_t ciudadActualX;
    uint16_t ciudadActualY;

    void aplicarEnPunto(const QPoint& punto);
    // Traduce un punto en pixeles a celda; devuelve false si cae fuera del mapa.
    bool celdaEn(const QPoint& punto, uint16_t& x, uint16_t& y) const;
    int anchoCelda() const;
    int altoCelda() const;
};

#endif
