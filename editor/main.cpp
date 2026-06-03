#include <QApplication>

#include "editor_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    EditorWindow ventana;
    ventana.show();

    return app.exec();
}
