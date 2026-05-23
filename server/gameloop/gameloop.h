#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <list>

#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../game/config/lector_config_toml.h"
#include "../game/evento/evento_salida.h"
#include "../game/juego.h"
#include "comando_cliente.h"
#include "evento_sesion.h"
#include "monitor_clientes.h"

class Gameloop: public Thread {
private:
    Queue<ComandoCliente> colaComandos;
    Queue<EventoSesion>   colaEventosSesion;
    MonitorClientes& monitor;
    Juego juego;
    int tickMs;

    void procesarEventosSesion();
    void procesarComandos();
    void procesarComando(const ComandoCliente& comandoCliente);
    void despachar(const std::list<EventoSalida>& eventos);

public:
    Gameloop(MonitorClientes& monitor, ConfigCompleta config);

    void run() override;
    void detener();

    Queue<ComandoCliente>& getColaComandos();
    Queue<EventoSesion>&   getColaEventosSesion();
};

#endif
