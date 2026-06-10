#include "panel_elementos.h"

#include <QColor>
#include <QDrag>
#include <QFont>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

PanelElementos::PanelElementos(const CatalogoEditor* catalogo, QWidget* parent):
        QWidget(parent), catalogo(catalogo),
        seccion(SeccionCatalogo::Criaturas), indice(0),
        flechaIzq(":/mapas/flecha_izq.bmp"), flechaDer(":/mapas/flecha_der.bmp") {
    setMinimumSize(180, 112);
}

const std::vector<ElementoCatalogo>& PanelElementos::elementos() const {
    return catalogo->elementosDe(seccion);
}

SeccionCatalogo PanelElementos::seccionActual() const {
    return seccion;
}

QString PanelElementos::nombreSeccion() const {
    switch (seccion) {
        case SeccionCatalogo::Criaturas: return "CRIATURAS";
        case SeccionCatalogo::Npc:       return "NPC";
        case SeccionCatalogo::Pisos:     return "PISOS";
    }
    return "";
}

bool PanelElementos::elementoActual(ElementoCatalogo& out) const {
    const std::vector<ElementoCatalogo>& lista = elementos();
    if (lista.empty() || indice < 0 || indice >= static_cast<int>(lista.size())) {
        return false;
    }
    out = lista[indice];
    return true;
}

QRect PanelElementos::rectSeccionIzq() const { return QRect(4, 3, 21, 19); }
QRect PanelElementos::rectSeccionDer() const { return QRect(width() - 25, 3, 21, 19); }
QRect PanelElementos::rectElementoIzq() const { return QRect(40, height() - 22, 21, 19); }
QRect PanelElementos::rectElementoDer() const { return QRect(width() - 61, height() - 22, 21, 19); }
QRect PanelElementos::rectIcono() const { return QRect(40, 32, width() - 80, 50); }

void PanelElementos::cambiarSeccion(int delta) {
    // Orden ciclico: Criaturas -> Npc -> Pisos -> Criaturas.
    int s = static_cast<int>(seccion) + delta;
    const int total = 3;
    s = ((s % total) + total) % total;
    seccion = static_cast<SeccionCatalogo>(s);
    indice = 0;
    update();
    emit seleccionCambiada();
}

void PanelElementos::cambiarElemento(int delta) {
    const std::vector<ElementoCatalogo>& lista = elementos();
    if (lista.empty()) {
        return;
    }
    const int total = static_cast<int>(lista.size());
    indice = ((indice + delta) % total + total) % total;
    update();
    emit seleccionCambiada();
}

void PanelElementos::iniciarArrastre() {
    ElementoCatalogo elem;
    if (!elementoActual(elem)) {
        return;
    }
    const QString prefijo =
            (elem.seccion == SeccionCatalogo::Npc) ? "npc" : "criatura";

    QMimeData* mime = new QMimeData();
    mime->setData("application/x-argentum-elemento",
                  (prefijo + ":" + elem.clave).toUtf8());

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mime);
    if (!elem.icono.isNull()) {
        const QPixmap icono = elem.icono.scaledToHeight(40, Qt::SmoothTransformation);
        drag->setPixmap(icono);
        drag->setHotSpot(QPoint(icono.width() / 2, icono.height() / 2));
    }
    drag->exec(Qt::CopyAction);
}

void PanelElementos::mousePressEvent(QMouseEvent* event) {
    const QPoint p = event->position().toPoint();

    if (rectSeccionIzq().contains(p)) { cambiarSeccion(-1); return; }
    if (rectSeccionDer().contains(p)) { cambiarSeccion(1);  return; }
    if (rectElementoIzq().contains(p)) { cambiarElemento(-1); return; }
    if (rectElementoDer().contains(p)) { cambiarElemento(1);  return; }
    if (rectIcono().contains(p)) { iniciarArrastre(); return; }
}

void PanelElementos::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // Tapar el rotulo "CRIATURAS" del bmp y escribir la seccion actual.
    painter.fillRect(QRect(26, 2, width() - 52, 22), QColor(23, 24, 18));
    QFont fuenteTitulo("Serif", 11, QFont::Bold);
    painter.setFont(fuenteTitulo);
    painter.setPen(QColor(206, 184, 130));
    painter.drawText(QRect(26, 2, width() - 52, 22), Qt::AlignCenter, nombreSeccion());

    // Flechas de cambio de seccion (sobre el rotulo).
    if (!flechaIzq.isNull()) painter.drawPixmap(rectSeccionIzq(), flechaIzq);
    if (!flechaDer.isNull()) painter.drawPixmap(rectSeccionDer(), flechaDer);

    // Tapar "Nombre / Cantidad" del bmp con el fondo oscuro del cuadro.
    painter.fillRect(QRect(6, 28, width() - 12, height() - 30), QColor(7, 7, 7));

    const std::vector<ElementoCatalogo>& lista = elementos();
    if (lista.empty()) {
        painter.setPen(QColor(150, 150, 150));
        painter.setFont(QFont("Serif", 9));
        painter.drawText(QRect(0, 28, width(), height() - 30), Qt::AlignCenter,
                         "(proximamente)");
        return;
    }

    // Icono del elemento centrado en el slot, manteniendo proporcion.
    const ElementoCatalogo& elem = lista[indice];
    if (!elem.icono.isNull()) {
        const QRect destino = rectIcono();
        QPixmap escalado = elem.icono.scaled(destino.size(), Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
        const int x = destino.x() + (destino.width() - escalado.width()) / 2;
        const int y = destino.y() + (destino.height() - escalado.height()) / 2;
        painter.drawPixmap(x, y, escalado);
    }

    // Flechas para cambiar de elemento dentro de la seccion (abajo del slot).
    if (!flechaIzq.isNull()) painter.drawPixmap(rectElementoIzq(), flechaIzq);
    if (!flechaDer.isNull()) painter.drawPixmap(rectElementoDer(), flechaDer);
}
