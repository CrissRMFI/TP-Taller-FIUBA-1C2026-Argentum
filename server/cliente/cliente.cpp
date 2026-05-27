#include "cliente.h"

#include <iostream>
#include <utility>
#include <sys/socket.h>

#include "common/socket/liberror.h"
#include "server/enviador.h"
#include "server/recibidor.h"
#include "server/gameloop/monitor_clientes.h"

Cliente::Cliente(uint16_t idCliente, 
                                 std::unique_ptr<ProtocoloServidor> protocolo_servidor,
                                 Queue<ComandoCliente>& colaComandos,
                                 MonitorClientes& monitor, Queue<EventoSesion>& colaEventos,
                                 handshakeInicial handshake)
        : idCliente(idCliente),
            protocolo_servidor(std::move(protocolo_servidor)),
            dataJugador(std::move(handshake)),
            colaSalida(),
            colaComandos(colaComandos),
            monitorClientes(monitor),
            colaEventos(colaEventos),
            estaActivo(true) {}



Queue<MensajeServidor>& Cliente::obtenerColaSalida(){
    return this->colaSalida;
}

void Cliente::run() {
    std::cout << "[cliente " << idCliente << "] conectado\n";
    colaEventos.push(EventoSesion{
    TipoEventoSesion::Conectar, idCliente,
        DatosSesion{
            dataJugador.nombre,
            dataJugador.clasePersonaje,
            dataJugador.raza}});
    monitorClientes.setCliente(idCliente, dataJugador.nombre, dataJugador.password);

    Enviador enviador(*protocolo_servidor, colaSalida);
    Recibidor recibidor(*protocolo_servidor, colaComandos, monitorClientes, idCliente);
    recibidor.start();
    enviador.start();
    recibidor.join();

    // El recibidor termino: el socket esta cerrado y el cliente se fue. Avisamos al dominio para que Juego mueva al jugador de jugadoresConectados a jugadoresDesconectados, despida la sesion activa en indiceNicksConectados (no la cuenta: el Jugador con su progreso sigue vivo en jugadoresDesconectados) y notifique al clan (regla 10.5). Si el gameloop ya cerro sus colas (shutdown del servidor), el push se ignora.
    try {
        colaEventos.push(EventoSesion{
                TipoEventoSesion::Desconectar, idCliente, DatosSesion{}});
    } catch (const ClosedQueue&) {}

    enviador.stop();
    enviador.join();
}

void Cliente::stop() {
    estaActivo = false;
    try {
        protocolo_servidor->cerrarConexion();
        //skt_client.close();
    }catch (const LibError&) {
        // ignoramos si el socket ya esta cerrado
    } catch (std::runtime_error&) {
        // cualquier otro error
    }
    std::cout << "[cliente " << idCliente << "] desconectado\n";
}
