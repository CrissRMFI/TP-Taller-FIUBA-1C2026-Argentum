#ifndef PANEL_ELEMENTOS_H
#define PANEL_ELEMENTOS_H

#include <QPixmap>
#include <QRect>
#include <QString>
#include <QWidget>

#include "catalogo_editor.h"


class PanelElementos : public QWidget {
    Q_OBJECT

public:
    explicit PanelElementos(const CatalogoEditor* catalogo, QWidget* parent = nullptr);

    bool elementoActual(ElementoCatalogo& out) const;
    SeccionCatalogo seccionActual() const;

signals:
    void seleccionCambiada();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    const CatalogoEditor* catalogo;
    SeccionCatalogo seccion;
    int indice;
    QPixmap flechaIzq;
    QPixmap flechaDer;

    const std::vector<ElementoCatalogo>& elementos() const;
    QString nombreSeccion() const;

    QRect rectSeccionIzq() const;
    QRect rectSeccionDer() const;
    QRect rectElementoIzq() const;
    QRect rectElementoDer() const;
    QRect rectIcono() const;

    void cambiarSeccion(int delta);
    void cambiarElemento(int delta);
    void iniciarArrastre();
};

#endif
