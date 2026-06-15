#include <QApplication>

#include "dialogo_inicio.h"
#include "editor_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    while (true) {
        DialogoInicio inicio;
        if (inicio.exec() != QDialog::Accepted) {
            return 0;
        }

        EditorWindow ventana(inicio.opcion(), inicio.rutaElegida());
        ventana.show();
        app.exec();

        if (!ventana.pidioVolver()) {
            return 0;
        }
    }
}
