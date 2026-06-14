#include "gameloop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include "traductor_protocolo.h"

template <typename T>
static void cerrarColaSiCorresponde(Queue<T>& cola, std::atomic_bool& cerrada) {
    if (cerrada.exchange(true)) {
        return;
    }

    cola.close();
}

Gameloop::Gameloop(MonitorClientes& monitor, ConfigCompleta config, Mundo&& mundo)
    : colaComandos(), colaEventosSesion(), monitor(monitor),
      juego(config.juego, std::move(config.items), std::move(config.hechizos),
            std::move(config.criaturas), std::move(mundo)),
      tickMs(config.juego.tickMs),
      guardadoSeg(config.juego.guardadoSeg),
      colaComandosCerrada(false),
      colaEventosSesionCerrada(false) {
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
    float acumuladorGuardado = 0.0f;

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

        // Guardado periodico de los conectados.
        if (guardadoSeg > 0) {
            acumuladorGuardado += deltaSegundos;
            if (acumuladorGuardado >= static_cast<float>(guardadoSeg)) {
                juego.persistirConectados();
                acumuladorGuardado = 0.0f;
            }
        }

        std::this_thread::sleep_until(siguienteTick);
        siguienteTick += duracionTick;

        if (Clock::now() > siguienteTick + duracionTick) {
            siguienteTick = Clock::now() + duracionTick;
        }
    }

    // Al apagarse, persistir a todos para no perder progreso (desconexion abrupta
    // del servidor).
    juego.persistirTodos();
}

void Gameloop::detener() {
    stop();

    cerrarColaSiCorresponde(colaComandos, colaComandosCerrada);
    cerrarColaSiCorresponde(colaEventosSesion, colaEventosSesionCerrada);
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
                                                evento.datos.cabeza,
                                                evento.datos.cuerpo));
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
    for (const EventoSalida& evento : eventos) {
        monitor.despachar(TraductorProtocolo::traducir(evento));
    }
}
