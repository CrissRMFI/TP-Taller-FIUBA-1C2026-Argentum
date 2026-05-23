#include "conexion_servidor.h"

#include <iostream>

ConexionServidor::ConexionServidor(const char* hostname, const char* servname)
    : protocolo(Socket(hostname, servname)),
      receptor(protocolo, &colaEntrante),
      emisor(protocolo, &colaSaliente) {}

void ConexionServidor::iniciar() {
    receptor.start();
    emisor.start();
}

void ConexionServidor::detener() {
    receptor.stop();
    emisor.stop();
    protocolo.cerrarConexion();
    colaSaliente.close();
}

void ConexionServidor::esperar() {
    receptor.join();
    emisor.join();
}

bool ConexionServidor::estaActivo() const {
    return receptor.is_alive() || emisor.is_alive();
}
