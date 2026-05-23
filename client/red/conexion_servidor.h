#ifndef CONEXION_SERVIDOR_H
#define CONEXION_SERVIDOR_H

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/socket/socket.h"
#include "../../common/thread/queue.h"
#include "../protocolo/protocolo_cliente.h"
#include "emisor.h"
#include "receptor.h"

class ConexionServidor {
private:
    ProtocoloCliente protocolo;
    Queue<MensajeServidor> colaEntrante;
    Queue<ComandoJugador> colaSaliente;
    Receptor receptor;
    Emisor emisor;

public:
    ConexionServidor(const char* hostname, const char* servname);

    ConexionServidor(const ConexionServidor&) = delete;
    ConexionServidor& operator=(const ConexionServidor&) = delete;
    ConexionServidor(ConexionServidor&&) = delete;
    ConexionServidor& operator=(ConexionServidor&&) = delete;

    void iniciar();
    void detener();
    void esperar();
    bool estaActivo() const;

    Queue<MensajeServidor>* obtenerColaEntrante() { return &colaEntrante; }
    Queue<ComandoJugador>* obtenerColaSaliente() { return &colaSaliente; }

    ~ConexionServidor() = default;
};

#endif
