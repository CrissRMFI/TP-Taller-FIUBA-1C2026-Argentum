#ifndef BARRAS_STATS_H
#define BARRAS_STATS_H

#include <cstdint>

#include <QWidget>

#include "catalogo_editor.h"

// Rellena las barritas de stats del bmp (vida / danio / nivel / oro) segun la criatura seleccionada.
class BarrasStats : public QWidget {
    Q_OBJECT

public:
    explicit BarrasStats(const CatalogoEditor* catalogo, QWidget* parent = nullptr);

    void setStats(const ElementoCatalogo& elem);
    void limpiar();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    const CatalogoEditor* catalogo;

    bool hay;
    uint16_t vida;
    uint8_t danio;
    uint8_t nivel;
    uint32_t oro;

    uint16_t maxVida;
    uint8_t maxDanio;
    uint8_t maxNivel;
    uint32_t maxOro;

    void calcularMaximos();
    void dibujarBarra(QPainter& painter, const QRect& slot, double fraccion,
                      const QColor& color);
};

#endif
