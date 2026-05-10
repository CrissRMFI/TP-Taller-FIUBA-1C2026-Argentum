#ifndef PROTOCOLO_CLIENTE_H
#define PROTOCOLO_CLIENTE_H

#include <cstdint>

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/protocolo/protocolo.h"

class ProtocoloCliente : public Protocolo {
public:
    explicit ProtocoloCliente(Socket&& skt);

    void enviarComando(const ComandoJugador& comando);

    MensajeServidor recibirMensaje();

    void cerrarConexion();

private:
    static constexpr uint16_t MAX_NICK = 32;
    static constexpr uint16_t MAX_CHAT = 256;
    static constexpr uint16_t MAX_CLAN = 32;
    static constexpr uint8_t MAX_DIRECCION = 3;

    static constexpr uint8_t MAX_TIPO_ENTIDAD = 2;
    static constexpr uint8_t MAX_ESTADO_ENTIDAD = 2;
    static constexpr uint8_t MAX_ESQUIVADOR = 1;
    static constexpr uint8_t MAX_TIPO_CLAN = 4;
    static constexpr uint16_t MAX_CANTIDAD_UINT8 = 255;

    void validarDireccion(uint8_t direccion) const;
    void validarCantidad(uint16_t cantidad) const;

    void validarTipoEntidad(uint8_t tipo) const;
    void validarEstadoEntidad(uint8_t estado) const;
    void validarEsquivador(uint8_t esquivador) const;
    void validarTipoClan(uint8_t tipo) const;

    void enviarComandoMover(const ComandoMover& comando);
    void enviarComandoAtacar(const ComandoAtacar& comando);
    void enviarComandoMeditar(const ComandoMeditar& comando);
    void enviarComandoResucitar(const ComandoResucitar& comando);
    void enviarComandoCurar(const ComandoCurar& comando);
    void enviarComandoTomar(const ComandoTomar& comando);
    void enviarComandoTirar(const ComandoTirar& comando);
    void enviarComandoEquipar(const ComandoEquipar& comando);
    void enviarComandoComprar(const ComandoComprar& comando);
    void enviarComandoVender(const ComandoVender& comando);
    void enviarComandoDepositarItem(const ComandoDepositarItem& comando);
    void enviarComandoDepositarOro(const ComandoDepositarOro& comando);
    void enviarComandoRetirarItem(const ComandoRetirarItem& comando);
    void enviarComandoRetirarOro(const ComandoRetirarOro& comando);
    void enviarComandoListar(const ComandoListar& comando);
    void enviarComandoChatGlobal(const ComandoChatGlobal& comando);
    void enviarComandoChatPrivado(const ComandoChatPrivado& comando);
    void enviarComandoFundarClan(const ComandoFundarClan& comando);
    void enviarComandoUnirseClan(const ComandoUnirseClan& comando);
    void enviarComandoRevisarClan(const ComandoRevisarClan& comando);
    void enviarComandoClanAceptar(const ComandoGestionMiembreClan& comando);
    void enviarComandoClanRechazar(const ComandoGestionMiembreClan& comando);
    void enviarComandoClanBan(const ComandoGestionMiembreClan& comando);
    void enviarComandoClanKick(const ComandoGestionMiembreClan& comando);
    void enviarComandoDejarClan(const ComandoDejarClan& comando);

    MensajeServidor recibirEstadoPersonaje();
    MensajeServidor recibirPosicionEntidad();
    MensajeServidor recibirEntidadDesaparecio();
    MensajeServidor recibirDanoRecibido();
    MensajeServidor recibirDanoProducido();
    MensajeServidor recibirEsquive();
    MensajeServidor recibirMuerteEntidad();
    MensajeServidor recibirItemEnSuelo();
    MensajeServidor recibirItemDesaparecioSuelo();
    MensajeServidor recibirActualizarInventario();
    MensajeServidor recibirActualizarEquipamiento();
    MensajeServidor recibirMensajeChat();
    MensajeServidor recibirMensajeClan();
    MensajeServidor recibirResucitado();
    MensajeServidor recibirListaItems();
    MensajeServidor recibirErrorAccion();
};

#endif
