#include "cliente.h"

#include <iostream>
#include <utility>
#include <sys/socket.h>

#include "common/socket/liberror.h"
#include "server/enviador.h"
#include "server/recibidor.h"
#include "server/gameloop/monitor_clientes.h"

Cliente::Cliente(uint16_t idCliente,
                 Socket&& skt,
                 Queue<ComandoJugador>& colaComandos,
                 MonitorClientes& monitor)
    : idCliente(idCliente),
      protocolo_servidor(std::move(skt)),
      colaComandos(colaComandos),
      monitorClientes(monitor) {
}

uint16_t Cliente::id() const {
    return idCliente;
}
Queue<MensajeServidor>& Cliente::obtenerColaSalida(){
    return this->colaSalida;
}


void Cliente::run() {
    std::cout << "[cliente " << idCliente << "] conectado\n";

    //ProtocoloServidor protocolo_servidor(protocolo_servidor);
    Enviador enviador(protocolo_servidor, colaSalida);
    Recibidor recibidor(protocolo_servidor, colaComandos, monitorClientes, idCliente);
    recibidor.start();
    enviador.start();
    recibidor.join();
    enviador.stop();
    enviador.join();
}

void Cliente::stop() {
    estaActivo = false;
    try {
        protocolo_servidor.cerrarConexion();
        //skt_client.close();
    }catch (const LibError&) {
        // ignoramos si el socket ya esta cerrado
    } catch (std::runtime_error&) {
        // cualquier otro error
    }
}
