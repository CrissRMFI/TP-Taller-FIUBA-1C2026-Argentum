#include "gameloop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include "traductor_protocolo.h"

Gameloop::Gameloop(MonitorClientes& monitor, ConfigCompleta config)
    : colaComandos(), colaEventosSesion(), monitor(monitor),
      juego(config.juego, std::move(config.items)),
      tickMs(config.juego.tickMs) {
    if (tickMs <= 0) {
        throw std::invalid_argument("El tick del gameloop debe ser mayor a cero");
    }
}

void Gameloop::run() {
    using Clock = std::chrono::steady_clock;

    const auto duracionTick = std::chrono::milliseconds(tickMs);
    // Tolerancia máxima de lag: 2 ticks del servidor
    const float deltaMaxSegundos = std::chrono::duration<float>(duracionTick).count() * 2.0f;
    auto instanteAnterior = Clock::now();
    auto siguienteTick = instanteAnterior + duracionTick;

    while (should_keep_running()) {
        auto ahora = Clock::now();
        float deltaSegundos = std::chrono::duration<float>(ahora - instanteAnterior).count();
        if (deltaSegundos > deltaMaxSegundos) {
            deltaSegundos = deltaMaxSegundos;
        }
        instanteAnterior = ahora;

        procesarEventosSesion();
        procesarComandos();

        auto mensajes = juego.actualizar(deltaSegundos);
        despachar(mensajes);

        std::this_thread::sleep_until(siguienteTick);
        siguienteTick += duracionTick;

        if (Clock::now() > siguienteTick + duracionTick) {
            siguienteTick = Clock::now() + duracionTick;
        }
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
    std::list<EventoSalida> eventos =
        juego.ejecutarComando(comandoCliente.idCliente, comandoCliente.comando);

    despachar(eventos);
}

void Gameloop::despachar(const std::list<EventoSalida>& eventos) {
    // Traducimos cada evento de dominio al wire format del protocolo
    // recién acá. `Juego` no conoce Opcode ni structs de `MensajeServidor`.
    for (const EventoSalida& evento : eventos) {
        monitor.despachar(TraductorProtocolo::traducir(evento));
    }
}
