#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <list>

#include "../../common/thread/queue.h"
#include "../../common/thread/thread.h"
#include "../game/juego.h"
#include "comando_cliente.h"
#include "mensaje_salida.h"
#include "monitor_clientes.h"

class Gameloop: public Thread {
private:
    Queue<ComandoCliente> colaComandos;
    MonitorClientes& monitor;
    Juego juego;

    void procesarComandos();
    void procesarComando(const ComandoCliente& comandoCliente);
    void despachar(const std::list<MensajeSalida>& mensajes);

public:
    explicit Gameloop(MonitorClientes& monitor);

    void run() override;
    void detener();

    Queue<ComandoCliente>& getColaComandos();
};

#endif
