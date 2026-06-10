#include "mapa_canvas.h"

#include <algorithm>

#include <QColor>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QList>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

MapaCanvas::MapaCanvas(EditorMapa* modelo, const CatalogoEditor* catalogo, QWidget* parent):
        QWidget(parent), modelo(modelo), catalogo(catalogo),
        herramienta(HerramientaCanvas::Ciudad),
        dibujandoCiudad(false), ciudadInicioX(0), ciudadInicioY(0),
        ciudadActualX(0), ciudadActualY(0) {
    setMinimumSize(300, 300);
    setAcceptDrops(true);
}

void MapaCanvas::setHerramienta(HerramientaCanvas nueva) {
    herramienta = nueva;
}

int MapaCanvas::anchoCelda() const {
    return std::max(1, width() / std::max<int>(1, modelo->getAncho()));
}

int MapaCanvas::altoCelda() const {
    return std::max(1, height() / std::max<int>(1, modelo->getAlto()));
}

void MapaCanvas::dibujarFigura(QPainter& painter, const QPixmap& icono, uint16_t celdaX, uint16_t celdaY, int celdaW, int celdaH) {
    
    if (icono.isNull()) {
        return;
    }
    const int lado = std::max(celdaW, celdaH) * 2;
    QPixmap escalado = icono.scaled(lado, lado, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const int cx = celdaX * celdaW + celdaW / 2;
    const int cy = celdaY * celdaH + celdaH / 2;
    painter.drawPixmap(cx - escalado.width() / 2, cy - escalado.height() / 2, escalado);
}

void MapaCanvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    const int celdaW = anchoCelda();
    const int celdaH = altoCelda();

    painter.fillRect(rect(), QColor(60, 120, 60));  // pasto

    // Ciudades (zonas seguras)
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(80, 160, 220, 90));
    for (const Ciudad& c : modelo->getCiudades()) {
        const int x = c.xMin * celdaW;
        const int y = c.yMin * celdaH;
        const int w = (c.xMax - c.xMin + 1) * celdaW;
        const int h = (c.yMax - c.yMin + 1) * celdaH;
        painter.drawRect(x, y, w, h);
    }

    // Paredes.
    painter.setBrush(QColor(20, 20, 20));
    for (const Posicion& p : modelo->getParedes()) {
        painter.drawRect(p.x * celdaW, p.y * celdaH, celdaW, celdaH);
    }

    // NPCs
    for (const NpcEditor& n : modelo->getNpcs()) {
        const QPixmap icono = catalogo->iconoNpc(n.tipo);
        if (icono.isNull()) {
            painter.setBrush(QColor(160, 60, 200));
            painter.drawEllipse(n.x * celdaW, n.y * celdaH, celdaW, celdaH);
        } else {
            dibujarFigura(painter, icono, n.x, n.y, celdaW, celdaH);
        }
    }

    // Criaturas
    for (const CriaturaEditor& c : modelo->getCriaturas()) {
        const QPixmap icono = catalogo->iconoCriatura(c.tipo);
        if (icono.isNull()) {
            painter.setBrush(QColor(200, 60, 60));
            painter.drawEllipse(c.x * celdaW, c.y * celdaH, celdaW, celdaH);
        } else {
            dibujarFigura(painter, icono, c.x, c.y, celdaW, celdaH);
        }
    }

    // Grilla
    if (celdaW >= 4 && celdaH >= 4) {
        painter.setPen(QColor(0, 0, 0, 40));
        for (uint16_t cx = 0; cx <= modelo->getAncho(); ++cx) {
            painter.drawLine(cx * celdaW, 0, cx * celdaW, modelo->getAlto() * celdaH);
        }
        for (uint16_t cy = 0; cy <= modelo->getAlto(); ++cy) {
            painter.drawLine(0, cy * celdaH, modelo->getAncho() * celdaW, cy * celdaH);
        }
    }

    // Preview del rectangulo de ciudad mientras se arrastra.
    if (dibujandoCiudad) {
        const uint16_t cxMin = std::min(ciudadInicioX, ciudadActualX);
        const uint16_t cyMin = std::min(ciudadInicioY, ciudadActualY);
        const uint16_t cxMax = std::max(ciudadInicioX, ciudadActualX);
        const uint16_t cyMax = std::max(ciudadInicioY, ciudadActualY);
        painter.setBrush(QColor(80, 160, 220, 110));
        painter.setPen(QColor(40, 100, 160));
        painter.drawRect(cxMin * celdaW, cyMin * celdaH,
                         (cxMax - cxMin + 1) * celdaW, (cyMax - cyMin + 1) * celdaH);
    }
}

bool MapaCanvas::celdaEn(const QPoint& punto, uint16_t& x, uint16_t& y) const {
    if (punto.x() < 0 || punto.y() < 0) {
        return false;
    }
    const int col = punto.x() / anchoCelda();
    const int fila = punto.y() / altoCelda();
    if (col >= modelo->getAncho() || fila >= modelo->getAlto()) {
        return false;
    }
    x = static_cast<uint16_t>(col);
    y = static_cast<uint16_t>(fila);
    return true;
}

void MapaCanvas::aplicarTerreno(const QPoint& punto) {
    uint16_t x = 0;
    uint16_t y = 0;
    if (!celdaEn(punto, x, y)) {
        return;
    }
    if (herramienta == HerramientaCanvas::Pared) {
        modelo->ponerPared(x, y);
        update();
    }
}

void MapaCanvas::colocarDesdeMime(const QByteArray& data, const QPoint& punto) {
    uint16_t x = 0;
    uint16_t y = 0;
    if (!celdaEn(punto, x, y)) {
        return;
    }
    const QString texto = QString::fromUtf8(data);
    const int sep = texto.indexOf(':');
    if (sep < 0) {
        return;
    }
    const QString prefijo = texto.left(sep);
    const QString clave = texto.mid(sep + 1);

    if (prefijo == "criatura") {
        TipoCriatura tipo;
        if (catalogo->criaturaPorClave(clave, tipo)) {
            modelo->ponerCriatura(tipo, x, y);
        }
    } else if (prefijo == "npc") {
        TipoNpc tipo;
        if (catalogo->npcPorClave(clave, tipo)) {
            modelo->ponerNpc(tipo, x, y);
        }
    }
    update();
}

void MapaCanvas::mousePressEvent(QMouseEvent* event) {
    const QPoint punto = event->position().toPoint();

    if (event->button() == Qt::RightButton) {
        uint16_t x = 0;
        uint16_t y = 0;
        if (celdaEn(punto, x, y)) {
            modelo->borrarEn(x, y);
            update();
        }
        return;
    }

    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (herramienta == HerramientaCanvas::Ciudad) {
        uint16_t x = 0;
        uint16_t y = 0;
        if (celdaEn(punto, x, y)) {
            dibujandoCiudad = true;
            ciudadInicioX = ciudadActualX = x;
            ciudadInicioY = ciudadActualY = y;
            update();
        }
        return;
    }

    aplicarTerreno(punto);
}

void MapaCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if (dibujandoCiudad) {
        uint16_t x = 0;
        uint16_t y = 0;
        if (celdaEn(event->position().toPoint(), x, y)) {
            ciudadActualX = x;
            ciudadActualY = y;
            update();
        }
        return;
    }

    aplicarTerreno(event->position().toPoint());
}

void MapaCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && dibujandoCiudad) {
        modelo->agregarCiudad(ciudadInicioX, ciudadInicioY, ciudadActualX, ciudadActualY);
        dibujandoCiudad = false;
        update();
    }
}

void MapaCanvas::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-argentum-elemento")) {
        event->acceptProposedAction();
    }
}

void MapaCanvas::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasFormat("application/x-argentum-elemento")) {
        event->acceptProposedAction();
    }
}

void MapaCanvas::dropEvent(QDropEvent* event) {
    const QMimeData* mime = event->mimeData();
    if (!mime->hasFormat("application/x-argentum-elemento")) {
        return;
    }
    colocarDesdeMime(mime->data("application/x-argentum-elemento"),
                     event->position().toPoint());
    event->acceptProposedAction();
}
