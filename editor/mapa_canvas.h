#ifndef MAPA_CANVAS_H
#define MAPA_CANVAS_H

#include <QByteArray>
#include <QPoint>
#include <QWidget>

#include "catalogo_editor.h"
#include "editor_mapa.h"


enum class HerramientaCanvas {
    Pared,
    Ciudad,
};

class MapaCanvas : public QWidget {
    Q_OBJECT

public:
    MapaCanvas(EditorMapa* modelo, const CatalogoEditor* catalogo, QWidget* parent = nullptr);

    void setHerramienta(HerramientaCanvas herramienta);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    EditorMapa* modelo;
    const CatalogoEditor* catalogo;
    HerramientaCanvas herramienta;

    bool dibujandoCiudad;
    uint16_t ciudadInicioX;
    uint16_t ciudadInicioY;
    uint16_t ciudadActualX;
    uint16_t ciudadActualY;

    void aplicarTerreno(const QPoint& punto);
    void colocarDesdeMime(const QByteArray& data, const QPoint& punto);
    void dibujarFigura(QPainter& painter, const QPixmap& icono, uint16_t celdaX, uint16_t celdaY, int celdaW, int celdaH);

    bool celdaEn(const QPoint& punto, uint16_t& x, uint16_t& y) const;
    int anchoCelda() const;
    int altoCelda() const;
};

#endif
