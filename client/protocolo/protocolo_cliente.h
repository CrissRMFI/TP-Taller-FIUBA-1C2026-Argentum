#ifndef PROTOCOLO_CLIENTE_H
#define PROTOCOLO_CLIENTE_H

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "../../common/protocolo/comando_jugador.h"
#include "../../common/protocolo/estado_entidad.h"
#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/protocolo/protocolo.h"
#include "common/protocolo/dato_sesion_cliente.h"

class ProtocoloCliente : public Protocolo {
public:
    explicit ProtocoloCliente(Socket&& skt);

    void enviarComando(const ComandoJugador& comando);

    MensajeServidor recibirMensaje();
    void enviarUsuario(const handshakeInicial &dataJugador);
    MensajeServidor recibirEstadoUsuario();

    void cerrarConexion();

private:
    static constexpr uint16_t MAX_NICK = 32;
    static constexpr uint16_t MAX_CHAT = 256;
    static constexpr uint16_t MAX_CLAN = 32;
    static const std::unordered_set<uint8_t> DIRECCIONES_VALIDAS;

    static constexpr uint8_t MAX_TIPO_ENTIDAD = 2;
    static constexpr uint8_t MAX_ESTADO_ENTIDAD =
            static_cast<uint8_t>(EstadoEntidadProtocolo::Resucitando);
    static constexpr uint8_t MAX_ESQUIVADOR = 1;
    static constexpr uint8_t MAX_TIPO_CLAN = static_cast<uint8_t>(TipoMensajeClan::Abandono);
    static constexpr uint16_t MAX_CANTIDAD_UINT8 = 255;

    void validarDireccion(uint8_t direccion) const;
    void validarCantidad(uint16_t cantidad) const;

    void validarTipoEntidad(uint8_t tipo) const;
    void validarEstadoEntidad(uint8_t estado) const;
    void validarEsquivador(uint8_t esquivador) const;
    void validarTipoClan(uint8_t tipo) const;

    void enviarComandoEmpezarMover(const ComandoEmpezarMover& comando);
    void enviarComandoDetenerMover(const ComandoDetenerMover& comando);
    void enviarComandoAtacar(const ComandoAtacar& comando);
    void enviarComandoMeditar(const ComandoMeditar& comando);
    void enviarComandoResucitar(const ComandoResucitar& comando);
    
    void enviarComandoCurar(const ComandoCurar& comando);
    void enviarComandoTomar(const ComandoTomar& comando);
    void enviarComandoTirar(const ComandoTirar& comando);
    void enviarComandoEquipar(const ComandoEquipar& comando);
    void enviarComandoUsar(const ComandoUsar& comando);
    void enviarComandoComprarHechizo(const ComandoComprarHechizo& comando);
    void enviarComandoLanzarHechizo(const ComandoLanzarHechizo& comando);
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
    void enviarComandoCheat(const ComandoCheat& comando);



    MensajeServidor recibirEstadoPersonaje();
    MensajeServidor recibirPosicionEntidad();
    MensajeServidor recibirEntidadDesaparecio();
    MensajeServidor recibirDanoRecibido();
    MensajeServidor recibirDanoProducido();
    MensajeServidor recibirEsquive();
    MensajeServidor recibirMuerteEntidad();
    MensajeServidor recibirItemEnSuelo();
    MensajeServidor recibirItemDesaparecioSuelo();
    MensajeServidor recibirOroEnSuelo();
    MensajeServidor recibirOroDesaparecioSuelo();
    MensajeServidor recibirActualizarInventario();
    MensajeServidor recibirActualizarEquipamiento();
    MensajeServidor recibirMensajeChat();
    MensajeServidor recibirMensajeClan();
    MensajeServidor recibirResucitado();
    MensajeServidor recibirListaItems();
    MensajeServidor recibirContenidoBanco();
    MensajeServidor recibirListaHechizos();
    MensajeServidor recibirFxHechizo();
    MensajeServidor recibirProyectil();
    MensajeServidor recibirErrorAccion();
};

#endif
