#include "mapa_canvas.h"

#include <algorithm>

#include <QColor>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QWheelEvent>

MapaCanvas::MapaCanvas(EditorMapa* modelo, const CatalogoEditor* catalogo, QWidget* parent):
        QWidget(parent), modelo(modelo), catalogo(catalogo),
        pisoActivo(false), pisoClave(), pisoDestino(),
        dibujandoZona(false), zonaInicioX(0), zonaInicioY(0),
        zonaActualX(0), zonaActualY(0),
        zoomPx(0), offX(0), offY(0), paneando(false), panUltimo() {
    setMinimumSize(300, 300);
    setAcceptDrops(true);
}

void MapaCanvas::setPincelPiso(bool activo, const QString& clave, const QString& destino) {
    pisoActivo = activo;
    pisoClave = clave;
    pisoDestino = destino;
}

void MapaCanvas::setModelo(EditorMapa* nuevo) {
    modelo = nuevo;
    dibujandoZona = false;
    reencuadrar();
}

int MapaCanvas::zoomAjuste() const {
    const int fx = width() / std::max<int>(1, modelo->getAncho());
    const int fy = height() / std::max<int>(1, modelo->getAlto());
    return std::max(1, std::min(fx, fy));
}

int MapaCanvas::celdaLado() const {
    return (zoomPx > 0) ? zoomPx : zoomAjuste();
}

void MapaCanvas::limitarPaneo() {
    const int lado = celdaLado();
    
    const int sobranteX = modelo->getAncho() * lado - width();
    const int sobranteY = modelo->getAlto() * lado - height();
    offX = (sobranteX <= 0) ? sobranteX / 2 : std::clamp(offX, 0, sobranteX);
    offY = (sobranteY <= 0) ? sobranteY / 2 : std::clamp(offY, 0, sobranteY);
}

void MapaCanvas::reencuadrar() {
    zoomPx = 0;
    offX = 0;
    offY = 0;
    limitarPaneo();
    update();
}

void MapaCanvas::dibujarTileZona(QPainter& painter, const QString& clave,
                                 uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax,
                                 int celdaW, int celdaH) {
    const QRect destino(xMin * celdaW, yMin * celdaH,
                        (xMax - xMin + 1) * celdaW, (yMax - yMin + 1) * celdaH);
    const QPixmap tile = catalogo->tilePiso(clave);
    if (tile.isNull()) {
        // Fallback de color si falta el tile.
        const QColor color = (clave == "desierto") ? QColor(214, 192, 120)
                           : (clave == "arbol")    ? QColor(34, 90, 34)
                                                   : QColor(60, 120, 60);
        painter.fillRect(destino, color);
        return;
    }
    // Escalo el tile al tamano de una celda
    const QPixmap escalado = tile.scaled(std::max(1, celdaW), std::max(1, celdaH),
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawTiledPixmap(destino, escalado);
}

void MapaCanvas::pintarPisos(QPainter& painter, int celdaW, int celdaH) {
    dibujarTileZona(painter, QString::fromStdString(modelo->getPisoBase()), 0, 0,
                    modelo->getAncho() - 1, modelo->getAlto() - 1, celdaW, celdaH);
    for (const ZonaPiso& z : modelo->getPisos()) {
        dibujarTileZona(painter, QString::fromStdString(z.clave),
                        z.xMin, z.yMin, z.xMax, z.yMax, celdaW, celdaH);
    }
}

void MapaCanvas::dibujarFigura(QPainter& painter, const QPixmap& icono,
                               uint16_t celdaX, uint16_t celdaY, int celdaW, int celdaH) {
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
    painter.fillRect(rect(), QColor(20, 20, 20));
    painter.translate(-offX, -offY);
    const int celdaW = celdaLado();
    const int celdaH = celdaW;

    // Pisos (terreno) resueltos por zona, ultima gana.
    pintarPisos(painter, celdaW, celdaH);

    // Ciudades (zonas seguras): overlay semitransparente.
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(80, 160, 220, 90));
    for (const Ciudad& c : modelo->getCiudades()) {
        painter.drawRect(c.xMin * celdaW, c.yMin * celdaH,
                         (c.xMax - c.xMin + 1) * celdaW, (c.yMax - c.yMin + 1) * celdaH);
    }

    // Paredes: tile de pared si esta, sino oscuro.
    const QPixmap tilePared = catalogo->tilePiso("pared");
    for (const Posicion& p : modelo->getParedes()) {
        if (tilePared.isNull()) {
            painter.setBrush(QColor(20, 20, 20));
            painter.drawRect(p.x * celdaW, p.y * celdaH, celdaW, celdaH);
        } else {
            painter.drawPixmap(QRect(p.x * celdaW, p.y * celdaH, celdaW, celdaH), tilePared);
        }
    }

    for (const ObjetoEditor& o : modelo->getObjetos()) {
        const QPixmap icono = catalogo->iconoElemento(QString::fromStdString(o.clave));
        dibujarFigura(painter, icono, o.x, o.y, celdaW, celdaH);
    }

    // NPCs y criaturas.
    for (const NpcEditor& n : modelo->getNpcs()) {
        const QPixmap icono = catalogo->iconoNpc(n.tipo);
        if (icono.isNull()) {
            painter.setBrush(QColor(160, 60, 200));
            painter.drawEllipse(n.x * celdaW, n.y * celdaH, celdaW, celdaH);
        } else {
            dibujarFigura(painter, icono, n.x, n.y, celdaW, celdaH);
        }
    }
    for (const CriaturaEditor& c : modelo->getCriaturas()) {
        const QPixmap icono = catalogo->iconoCriatura(c.tipo);
        if (icono.isNull()) {
            painter.setBrush(QColor(200, 60, 60));
            painter.drawEllipse(c.x * celdaW, c.y * celdaH, celdaW, celdaH);
        } else {
            dibujarFigura(painter, icono, c.x, c.y, celdaW, celdaH);
        }
    }

    // Grilla.
    if (celdaW >= 4 && celdaH >= 4) {
        painter.setPen(QColor(0, 0, 0, 40));
        for (uint16_t cx = 0; cx <= modelo->getAncho(); ++cx) {
            painter.drawLine(cx * celdaW, 0, cx * celdaW, modelo->getAlto() * celdaH);
        }
        for (uint16_t cy = 0; cy <= modelo->getAlto(); ++cy) {
            painter.drawLine(0, cy * celdaH, modelo->getAncho() * celdaW, cy * celdaH);
        }
    }

    // Preview de la zona mientras se arrastra.
    if (dibujandoZona) {
        const uint16_t cxMin = std::min(zonaInicioX, zonaActualX);
        const uint16_t cyMin = std::min(zonaInicioY, zonaActualY);
        const uint16_t cxMax = std::max(zonaInicioX, zonaActualX);
        const uint16_t cyMax = std::max(zonaInicioY, zonaActualY);
        painter.setBrush(QColor(255, 255, 255, 70));
        painter.setPen(QColor(240, 240, 240));
        painter.drawRect(cxMin * celdaW, cyMin * celdaH,
                         (cxMax - cxMin + 1) * celdaW, (cyMax - cyMin + 1) * celdaH);
    }
}

bool MapaCanvas::celdaEn(const QPoint& punto, uint16_t& x, uint16_t& y) const {
    const int lado = celdaLado();
    const int mx = punto.x() + offX;  // a coordenadas de mapa (deshace el paneo)
    const int my = punto.y() + offY;
    if (mx < 0 || my < 0) {
        return false;
    }
    const int col = mx / lado;
    const int fila = my / lado;
    if (col >= modelo->getAncho() || fila >= modelo->getAlto()) {
        return false;
    }
    x = static_cast<uint16_t>(col);
    y = static_cast<uint16_t>(fila);
    return true;
}

void MapaCanvas::wheelEvent(QWheelEvent* event) {
    const int delta = event->angleDelta().y();
    if (delta == 0) {
        return;
    }
    // Zoom con Ctrl+rueda, centrado en el cursor (el punto bajo el mouse no se mueve).
   
    if (!(event->modifiers() & Qt::ControlModifier)) {
        return;
    }
    const QPoint cursor = event->position().toPoint();
    const int ladoViejo = celdaLado();
    const double mapaX = (cursor.x() + offX) / static_cast<double>(ladoViejo);
    const double mapaY = (cursor.y() + offY) / static_cast<double>(ladoViejo);
    const int ladoNuevo = std::clamp(ladoViejo + (delta > 0 ? 2 : -2), zoomAjuste(), 48);
    zoomPx = ladoNuevo;
    offX = static_cast<int>(mapaX * ladoNuevo - cursor.x());
    offY = static_cast<int>(mapaY * ladoNuevo - cursor.y());
    limitarPaneo();
    update();
    event->accept();
}

void MapaCanvas::aplicarZona(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    // Rutea la zona pintada segun el destino del piso elegido.
    if (pisoDestino == "pared") {
        modelo->pintarParedes(x1, y1, x2, y2);
    } else if (pisoDestino == "ciudad") {
        modelo->agregarCiudad(x1, y1, x2, y2);
    } else {
        modelo->pintarPiso(pisoClave.toStdString(), x1, y1, x2, y2);
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
        
        if (catalogo->esSoloMazmorra(clave) && !modelo->esMazmorra()) {
            emit aviso("Esa criatura es exclusiva de mazmorra: no va en el mapa exterior.");
            return;
        }
        TipoCriatura tipo;
        if (catalogo->criaturaPorClave(clave, tipo)) {
            modelo->ponerCriatura(tipo, x, y);
        }
    } else if (prefijo == "npc") {
        TipoNpc tipo;
        if (catalogo->npcPorClave(clave, tipo)) {
            modelo->ponerNpc(tipo, x, y);
        }
    } else if (prefijo == "elemento") {
        const QString piso = QString::fromStdString(modelo->pisoEn(x, y));
        if (!catalogo->pisoPermitido(clave, piso)) {
            ElementoCatalogo elem;
            const QString nombre =
                    catalogo->elementoPorClave(clave, elem) ? elem.nombre : clave;
            emit aviso(nombre + " no se puede colocar sobre " + piso + ".");
            return;
        }
        if (clave == "pared") {
            modelo->ponerPared(x, y);
        } else {
            modelo->ponerObjeto(clave.toStdString(), x, y);
        }
    }
    update();
}

void MapaCanvas::mousePressEvent(QMouseEvent* event) {
    const QPoint punto = event->position().toPoint();

    // Boton del medio (rueda)
    if (event->button() == Qt::MiddleButton) {
        paneando = true;
        panUltimo = punto;
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    if (event->button() == Qt::RightButton) {
        uint16_t x = 0;
        uint16_t y = 0;
        if (celdaEn(punto, x, y)) {
            modelo->borrarEn(x, y);
            update();
        }
        return;
    }

    // Pintar zona de terreno solo si hay un piso activo (seccion PISOS).
    if (event->button() == Qt::LeftButton && pisoActivo) {
        uint16_t x = 0;
        uint16_t y = 0;
        if (celdaEn(punto, x, y)) {
            dibujandoZona = true;
            zonaInicioX = zonaActualX = x;
            zonaInicioY = zonaActualY = y;
            update();
        }
    }
}

void MapaCanvas::mouseMoveEvent(QMouseEvent* event) {
    const QPoint punto = event->position().toPoint();

    // Paneo "manito": arrastra el mapa en cualquier direccion.
    if (paneando && (event->buttons() & Qt::MiddleButton)) {
        offX -= punto.x() - panUltimo.x();
        offY -= punto.y() - panUltimo.y();
        panUltimo = punto;
        limitarPaneo();
        update();
        return;
    }

    if (!(event->buttons() & Qt::LeftButton) || !dibujandoZona) {
        return;
    }
    uint16_t x = 0;
    uint16_t y = 0;
    if (celdaEn(punto, x, y)) {
        zonaActualX = x;
        zonaActualY = y;
        update();
    }
}

void MapaCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton && paneando) {
        paneando = false;
        unsetCursor();
        return;
    }
    if (event->button() == Qt::LeftButton && dibujandoZona) {
        aplicarZona(zonaInicioX, zonaInicioY, zonaActualX, zonaActualY);
        dibujandoZona = false;
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
