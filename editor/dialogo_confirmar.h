#ifndef DIALOGO_CONFIRMAR_H
#define DIALOGO_CONFIRMAR_H

#include <QDialog>

// Ventana "CONFIRMAR"
class DialogoConfirmar : public QDialog {
    Q_OBJECT

public:
    explicit DialogoConfirmar(QWidget* parent = nullptr);
};

#endif
