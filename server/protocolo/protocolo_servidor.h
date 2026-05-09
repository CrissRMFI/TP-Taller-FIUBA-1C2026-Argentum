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
    static constexpr uint16_t MAX_CLAN = 32;
    static constexpr uint8_t MAX_DIRECCION = 3;
    static constexpr uint8_t MAX_TIPO_ENTIDAD = 2;
    static constexpr uint8_t MAX_ESTADO_ENTIDAD = 2;
    static constexpr uint8_t MAX_ESQUIVADOR = 1;

    void validarDireccion(const uint8_t direccion) const;

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
    ComandoJugador recibirComandoFundarClan();
    ComandoJugador recibirComandoUnirseClan();
    ComandoJugador recibirComandoRevisarClan();
    ComandoJugador recibirComandoClanAceptar();
    ComandoJugador recibirComandoClanRechazar();
    ComandoJugador recibirComandoClanBan();
    ComandoJugador recibirComandoClanKick();
    ComandoJugador recibirComandoDejarClan();

    void validarTipoEntidad(uint8_t tipo) const;
    void validarEstadoEntidad(uint8_t estado) const;
    void validarEsquivador(uint8_t esquivador) const;


    void enviarEstadoPersonaje(const MensajeEstadoPersonaje &mensaje);
    void enviarPosicionEntidad(const MensajePosicionEntidad& mensaje);
    void enviarEntidadDesaparecio(const MensajeEntidadDesaparecio& mensaje);
    void enviarDanioRecibido(const MensajeDanoRecibido& mensaje);
    void enviarDanioProducido(const MensajeDanoProducido& mensaje);
    void enviarEsquive(const MensajeEsquive& mensaje);
    void enviarMuerteEntidad(const MensajeMuerteEntidad& mensaje);
};

#endif
