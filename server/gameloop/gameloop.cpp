#include "gameloop.h"

#include <stdexcept>
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
      loop(std::chrono::milliseconds(config.juego.tickMs)),
      colaComandosCerrada(false),
      colaEventosSesionCerrada(false) {
    if (tickMs <= 0) {
        throw std::invalid_argument("El tick del gameloop debe ser mayor a cero");
    }
}

void Gameloop::run() {
    float acumuladorGuardado = 0.0f;
    const float deltaSegundos = static_cast<float>(tickMs) / 1000.0f;

    loop.run([&](const uint32_t ticks_elapsed, const uint64_t) {
        if (!should_keep_running()) {
            loop.stop();
            return;
        }

        procesarEventosSesion();
        procesarComandos();

        // si se acumulan ticks, por cada tick atrasado se procesa un juego.actualizar()
        for (uint32_t i = 0; i < ticks_elapsed; ++i) {
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
        }
    });

    // Al apagarse, persistir a todos para no perder progreso (desconexion abrupta
    // del servidor).
    juego.persistirTodos();
}

void Gameloop::detener() {
    stop();
    loop.stop();

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
