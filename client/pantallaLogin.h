//
// Created by victoria zubieta on 25/05/2026.
//

#ifndef TALLER_TP_PANTALLALOGIN_H
#define TALLER_TP_PANTALLALOGIN_H
#pragma once
#include <string>
#include "../common/protocolo/dato_sesion_cliente.h"
#include "protocolo/protocolo_cliente.h"
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"


class PantallaLogin {
private:
    ProtocoloCliente& protocolo;
    bool preguntarCrear();
    std::string pedirNombre();
    ClasePersonaje pedirClase();
    Raza pedirRaza();

public:
    explicit PantallaLogin(ProtocoloCliente& protocolo);
    handshakeInicial ejecutar();

};
#endif //TALLER_TP_PANTALLALOGIN_H
