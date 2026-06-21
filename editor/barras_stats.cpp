#include "barras_stats.h"

#include <algorithm>

#include <QColor>
#include <QPainter>
#include <QRect>

BarrasStats::BarrasStats(const CatalogoEditor* catalogo, QWidget* parent) :
        QWidget(parent),
        catalogo(catalogo),
        hay(false),
        vida(0),
        danio(0),
        nivel(0),
        oro(0),
        maxVida(1),
        maxDanio(1),
        maxNivel(1),
        maxOro(1) {
    setAttribute(Qt::WA_TransparentForMouseEvents);  // no roba clicks al fondo
    calcularMaximos();
}

void BarrasStats::calcularMaximos() {
    for (const ElementoCatalogo& e : catalogo->elementosDe(SeccionCatalogo::Criaturas)) {
        if (!e.tieneStats) {
            continue;
        }
        maxVida = std::max(maxVida, e.vida);
        maxDanio = std::max<uint8_t>(maxDanio, e.danioMax);
        maxNivel = std::max<uint8_t>(maxNivel, e.nivel);
        maxOro = std::max<uint32_t>(maxOro, e.oro);
    }
}

void BarrasStats::setStats(const ElementoCatalogo& elem) {
    hay = elem.tieneStats;
    if (hay) {
        vida = elem.vida;
        danio = elem.danioMax;
        nivel = elem.nivel;
        oro = elem.oro;
    }
    update();
}

void BarrasStats::limpiar() {
    hay = false;
    update();
}

void BarrasStats::dibujarBarra(QPainter& painter, const QRect& slot, double fraccion,
                               const QColor& color) {
    fraccion = std::clamp(fraccion, 0.0, 1.0);
    const int ancho = static_cast<int>(slot.width() * fraccion);
    if (ancho <= 0) {
        return;
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(QRect(slot.x(), slot.y(), ancho, slot.height()), 3, 3);
}

void BarrasStats::paintEvent(QPaintEvent*) {
    if (!hay) {
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);


    const QRect slotVida(2, 3, 66, 10);
    const QRect slotDanio(80, 3, 72, 10);
    const QRect slotNivel(2, 29, 66, 10);
    const QRect slotOro(80, 29, 72, 10);

    dibujarBarra(painter, slotVida, vida / static_cast<double>(maxVida), QColor(210, 60, 60));
    dibujarBarra(painter, slotDanio, danio / static_cast<double>(maxDanio), QColor(210, 210, 225));
    dibujarBarra(painter, slotNivel, nivel / static_cast<double>(maxNivel), QColor(80, 200, 200));
    dibujarBarra(painter, slotOro, oro / static_cast<double>(maxOro), QColor(240, 210, 90));
}
