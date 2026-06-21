#include "test_utils.h"

#include <atomic>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

std::pair<Socket, Socket> crearParConectado() {
    static std::atomic<int> contador_puerto{17676};

    for (int intento = 0; intento < 100; ++intento) {
        int puerto = contador_puerto.fetch_add(1);
        std::string puerto_str = std::to_string(puerto);

        try {
            Socket listener(puerto_str.c_str());

            std::optional<Socket> cliente_skt;
            std::thread conectador([&cliente_skt, &puerto_str]() {
                try {
                    cliente_skt.emplace("localhost", puerto_str.c_str());
                } catch (...) {}
            });

            Socket servidor_skt = listener.accept();
            conectador.join();

            if (!cliente_skt) {
                continue;
            }

            return {std::move(servidor_skt), std::move(*cliente_skt)};
        } catch (...) {
            continue;
        }
    }

    throw std::runtime_error("No se pudo crear un par de sockets para testing");
}
