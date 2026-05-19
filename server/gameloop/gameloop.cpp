#include "gameloop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

Gameloop::Gameloop(MonitorClientes& monitor, ConfigCompleta config)
    : colaComandos(), colaEventosSesion(), monitor(monitor),
      juego(config.juego, std::move(config.items)),
      tickMs(config.juego.tickMs) {}

void Gameloop::run() {
    while (should_keep_running()) {
        procesarEventosSesion();
        procesarComandos();
        auto mensajes = juego.actualizar();
        despachar(mensajes);

        std::this_thread::sleep_for(std::chrono::milliseconds(tickMs));
    }
}

void Gameloop::detener() {
    stop();

    try {
        colaComandos.close();
    } catch (const std::runtime_error&) {}

    try {
        colaEventosSesion.close();
    } catch (const std::runtime_error&) {}
}

Queue<ComandoCliente>& Gameloop::getColaComandos() {
    return colaComandos;
}

Queue<EventoSesion>& Gameloop::getColaEventosSesion() {
    return colaEventosSesion;
}

void Gameloop::procesarEventosSesion() {
    EventoSesion evento;

    try {
        while (colaEventosSesion.try_pop(evento)) {
            if (evento.tipo == TipoEventoSesion::Conectar) {
                despachar(juego.conectarJugador(evento.idCliente,
                                                evento.datos.nombre,
                                                evento.datos.clase,
                                                evento.datos.raza,
                                                evento.datos.posicion));
            } else {
                despachar(juego.desconectarJugador(evento.idCliente));
            }
        }
    } catch (const ClosedQueue&) {}
}

void Gameloop::procesarComandos() {
    ComandoCliente comandoCliente;

    try {
        while (colaComandos.try_pop(comandoCliente)) {
            procesarComando(comandoCliente);
        }
    } catch (const ClosedQueue&) {}
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
