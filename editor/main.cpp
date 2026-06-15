#include <QApplication>

#include "dialogo_inicio.h"
#include "editor_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    DialogoInicio inicio;
    if (inicio.exec() != QDialog::Accepted) {
        return 0;
    }

    EditorWindow ventana(inicio.opcion(), inicio.rutaElegida());
    ventana.show();

    return app.exec();
}
