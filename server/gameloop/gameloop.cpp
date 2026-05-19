#include "gameloop.h"

#include <chrono>
#include <thread>
#include <utility>

Gameloop::Gameloop(MonitorClientes& monitor, ConfigCompleta config)
    : colaComandos(), colaEventosSesion(), monitor(monitor),
      juego(config.juego, std::move(config.items)) {}

void Gameloop::run() {
    while (should_keep_running()) {
        procesarEventosSesion();
        procesarComandos();
        auto mensajes = juego.actualizar();
        despachar(mensajes);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Gameloop::detener() {
    stop();
    colaComandos.close();
    colaEventosSesion.close();
}

Queue<ComandoCliente>& Gameloop::getColaComandos() {
    return colaComandos;
}

Queue<EventoSesion>& Gameloop::getColaEventosSesion() {
    return colaEventosSesion;
}

void Gameloop::procesarEventosSesion() {
    EventoSesion evento;
    while (colaEventosSesion.try_pop(evento)) {
        if (evento.tipo == TipoEventoSesion::Conectar) {
            juego.conectarJugador(evento.idCliente,
                                  evento.datos.nombre,
                                  evento.datos.clase,
                                  evento.datos.raza,
                                  evento.datos.posicion);
        } else {
            juego.desconectarJugador(evento.idCliente);
        }
    }
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
