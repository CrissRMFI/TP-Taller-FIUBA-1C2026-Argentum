#ifndef PROTOCOLO_SERVIDOR_H
#define PROTOCOLO_SERVIDOR_H

#include <cstdint>

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/protocolo/protocolo.h"
#include "../../common/protocolo/mensaje_servidor.h"

class ProtocoloServidor : public Protocolo {
  public:
    
    explicit ProtocoloServidor(Socket&& skt);

    ComandoJugador recibirComando();

     void enviarMensaje(const MensajeServidor& mensaje);

    void cerrarConexion();

  private:

    static constexpr uint16_t MAX_NICK = 32;
    static constexpr uint16_t MAX_CHAT = 256;
    
    ComandoJugador recibirComandoMover();
    ComandoJugador recibirComandoAtacar();
    ComandoJugador recibirComandoMeditar();
    ComandoJugador recibirComandoResucitar();
    ComandoJugador recibirComandoCurar();
    ComandoJugador recibirComandoTomar();
    ComandoJugador recibirComandoTirar();
    ComandoJugador recibirComandoEquipar();
    ComandoJugador recibirComandoComprar();
    ComandoJugador recibirComandoVender();
    ComandoJugador recibirComandoDepositarItem();
    ComandoJugador recibirComandoDepositarOro();
    ComandoJugador recibirComandoRetirarItem();
    ComandoJugador recibirComandoRetirarOro();
    ComandoJugador recibirComandoListar();
    ComandoJugador recibirComandoChatGlobal();
    ComandoJugador recibirComandoChatPrivado();
};

#endif
