#include <iostream>
#include <string>

#include "common/thread/queue.h"
#include "server/aceptador/aceptador.h"
#include "server/gameloop/comando_cliente.h"
#include "server/gameloop/monitor_clientes.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>\n";
        return 1;
    }
    try {
        Queue<ComandoCliente> colaComandos;
        MonitorClientes monitorClientes;

        Aceptador aceptador(argv[1], &colaComandos, &monitorClientes);
        aceptador.start();

        std::cout << "Servidor escuchando en puerto " << argv[1]
                  << ". 'q' + ENTER para salir.\n";

        std::string linea;
        while (std::getline(std::cin, linea)) {
            if (linea == "q") break;
        }

        aceptador.stop();
        aceptador.join();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << '\n';
        return 1;
    }
}
