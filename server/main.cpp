#include <string>

#include "server_.h"
#include "common/thread/queue.h"
#include "server/aceptador/aceptador.h"
#include "server/gameloop/comando_cliente.h"
#include "server/gameloop/monitor_clientes.h"
#include "server/game/registro_servidor.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        RegistroServidor::error("Uso: " + std::string(argv[0]) + " <puerto>");
        return 1;
    }
    try {

        const char* port = argv[1];
        Server server(port);
        server.run();
        return 0;
    } catch (const std::exception& e) {
        RegistroServidor::error(std::string("fatal: ") + e.what());
        return 1;
    }
}


//Queue<ComandoCliente> colaComandos;
//MonitorClientes monitorClientes;

// Aceptador aceptador(argv[1], &colaComandos, &monitorClientes);
// aceptador.start();
//
// std::cout << "Servidor escuchando en puerto " << argv[1]
//           << ". 'q' + ENTER para salir.\n";
//
// std::string linea;
// while (std::getline(std::cin, linea)) {
//     if (linea == "q") break;
// }
//
// aceptador.stop();
// aceptador.join();
