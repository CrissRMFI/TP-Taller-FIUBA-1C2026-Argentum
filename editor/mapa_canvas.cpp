#include "mapa_canvas.h"

#include <algorithm>

#include <QColor>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

MapaCanvas::MapaCanvas(EditorMapa* modelo, QWidget* parent):
        QWidget(parent), modelo(modelo), herramienta(HerramientaEditor::Pared),
        dibujandoCiudad(false), ciudadInicioX(0), ciudadInicioY(0),
        ciudadActualX(0), ciudadActualY(0) {
    setMinimumSize(400, 400);
}

void MapaCanvas::setHerramienta(HerramientaEditor nueva) {
    herramienta = nueva;
}

int MapaCanvas::anchoCelda() const {
    return std::max(1, width() / std::max<int>(1, modelo->getAncho()));
}

int MapaCanvas::altoCelda() const {
    return std::max(1, height() / std::max<int>(1, modelo->getAlto()));
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
        switch (n.tipo) {
            case TipoNpc::Sacerdote:   painter.setBrush(QColor(255, 255, 255)); break;
            case TipoNpc::Comerciante: painter.setBrush(QColor(160, 60, 200)); break;
            case TipoNpc::Banquero:    painter.setBrush(QColor(150, 150, 150)); break;
        }
        painter.drawEllipse(n.x * celdaW, n.y * celdaH, celdaW, celdaH);
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

void MapaCanvas::aplicarEnPunto(const QPoint& punto) {
    uint16_t x = 0;
    uint16_t y = 0;
    if (!celdaEn(punto, x, y)) {
        return;
    }

    switch (herramienta) {
        case HerramientaEditor::Pared:       modelo->ponerPared(x, y); break;
        case HerramientaEditor::Sacerdote:   modelo->ponerNpc(TipoNpc::Sacerdote, x, y); break;
        case HerramientaEditor::Comerciante: modelo->ponerNpc(TipoNpc::Comerciante, x, y); break;
        case HerramientaEditor::Banquero:    modelo->ponerNpc(TipoNpc::Banquero, x, y); break;
        case HerramientaEditor::Borrar:      modelo->borrarEn(x, y); break;
        case HerramientaEditor::Ciudad:      break;  // se maneja con arrastre
    }

    update();
}

void MapaCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (herramienta == HerramientaEditor::Ciudad) {
        uint16_t x = 0;
        uint16_t y = 0;
        if (celdaEn(event->position().toPoint(), x, y)) {
            dibujandoCiudad = true;
            ciudadInicioX = ciudadActualX = x;
            ciudadInicioY = ciudadActualY = y;
            update();
        }
        return;
    }

    aplicarEnPunto(event->position().toPoint());
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

    aplicarEnPunto(event->position().toPoint());
}

void MapaCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && dibujandoCiudad) {
        modelo->agregarCiudad(ciudadInicioX, ciudadInicioY, ciudadActualX, ciudadActualY);
        dibujandoCiudad = false;
        update();
    }
}
