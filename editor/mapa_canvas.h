#ifndef MAPA_CANVAS_H
#define MAPA_CANVAS_H

#include <QByteArray>
#include <QPoint>
#include <QString>
#include <QWidget>

#include "catalogo_editor.h"
#include "editor_mapa.h"

class MapaCanvas : public QWidget {
    Q_OBJECT

public:
    MapaCanvas(EditorMapa* modelo, const CatalogoEditor* catalogo, QWidget* parent = nullptr);

    void setPincelPiso(bool activo, const QString& clave, const QString& destino);

    // Ajustamos el zoom para ver todo el mapa y lo centramos en la vistana.
    void reencuadrar();

signals:
    void aviso(const QString& mensaje);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    EditorMapa* modelo;
    const CatalogoEditor* catalogo;

    bool pisoActivo;
    QString pisoClave;
    QString pisoDestino;

    // Estado del arrastre para pintar una zona rectangular (terreno).
    bool dibujandoZona;
    uint16_t zonaInicioX;
    uint16_t zonaInicioY;
    uint16_t zonaActualX;
    uint16_t zonaActualY;

    // Zoom 
    int zoomPx;
    int offX;
    int offY;

    // Paneo "manito" con el boton del medio (rueda) apretado.
    bool paneando;
    QPoint panUltimo;

    void colocarDesdeMime(const QByteArray& data, const QPoint& punto);
    void aplicarZona(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void pintarPisos(QPainter& painter, int celdaW, int celdaH);
    void dibujarTileZona(QPainter& painter, const QString& clave,
                         uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax,
                         int celdaW, int celdaH);
    void dibujarFigura(QPainter& painter, const QPixmap& icono,
                       uint16_t celdaX, uint16_t celdaY, int celdaW, int celdaH);

    bool celdaEn(const QPoint& punto, uint16_t& x, uint16_t& y) const;
    int celdaLado() const;
    int zoomAjuste() const;
    void limitarPaneo();
};

#endif
