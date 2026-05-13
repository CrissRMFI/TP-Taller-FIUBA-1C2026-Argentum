#include "gameloop.h"

#include <chrono>
#include <thread>

Gameloop::Gameloop(MonitorClientes& monitor): colaComandos(), monitor(monitor), juego() {}

void Gameloop::run() {
    while (should_keep_running()) {
        
        procesarComandos();
        // TODO:
        // auto mensajes = juego.actualizar();
        // despachar(mensajes);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Gameloop::detener() {
    stop();
    colaComandos.close();
}

Queue<ComandoCliente>& Gameloop::getColaComandos() {
    return colaComandos;
}

void Gameloop::procesarComandos() {
    ComandoCliente comandoCliente;

    while (colaComandos.try_pop(comandoCliente)) {
        procesarComando(comandoCliente);
    }
}

void Gameloop::procesarComando(const ComandoCliente& comandoCliente) {
    std::list<MensajeSalida> mensajes = juego.ejecutarComando(comandoCliente.idCliente,comandoCliente.comando
    );
    
    despachar(mensajes);
}

void Gameloop::despachar(const std::list<MensajeSalida>& mensajes) {
    for (const MensajeSalida& mensaje: mensajes) {
        monitor.despachar(mensaje);
    }
}
