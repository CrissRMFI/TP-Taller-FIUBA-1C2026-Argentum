#include "traductor_protocolo.h"

#include <type_traits>
#include <variant>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/protocolo/opcode.h"

namespace {

MensajeServidor aMensajeServidor(const EventoJuego& evento) {
    return std::visit([](const auto& e) -> MensajeServidor {
        using T = std::decay_t<decltype(e)>;

        if constexpr (std::is_same_v<T, EventoEstadoPersonaje>) {
            return { Opcode::ESTADO_PERSONAJE,
                     MensajeEstadoPersonaje{
                         e.vidaActual, e.vidaMax,
                         e.manaActual, e.manaMax,
                         e.oro, e.nivel, e.experiencia,
                         static_cast<uint8_t>(e.estado)} };
        } else if constexpr (std::is_same_v<T, EventoPosicionEntidad>) {
            return { Opcode::POSICION_ENTIDAD,
                     MensajePosicionEntidad{ e.id, e.x, e.y, e.tipo, e.estado } };
        } else if constexpr (std::is_same_v<T, EventoEntidadDesaparecio>) {
            return { Opcode::ENTIDAD_DESAPARECIO,
                     MensajeEntidadDesaparecio{ e.id } };
        } else if constexpr (std::is_same_v<T, EventoDanioRecibido>) {
            return { Opcode::DANIO_RECIBIDO,
                     MensajeDanoRecibido{ e.cantidad, e.idAtacante } };
        } else if constexpr (std::is_same_v<T, EventoDanioProducido>) {
            return { Opcode::DANIO_PRODUCIDO,
                     MensajeDanoProducido{ e.cantidad, e.idObjetivo } };
        } else if constexpr (std::is_same_v<T, EventoEsquive>) {
            return { Opcode::ESQUIVE,
                     MensajeEsquive{ e.idEntidad, e.esquivador } };
        } else if constexpr (std::is_same_v<T, EventoMuerteEntidad>) {
            return { Opcode::MUERTE_ENTIDAD,
                     MensajeMuerteEntidad{ e.id } };
        } else if constexpr (std::is_same_v<T, EventoItemEnSuelo>) {
            return { Opcode::ITEM_EN_SUELO,
                     MensajeItemEnSuelo{ e.idItem, e.x, e.y } };
        } else if constexpr (std::is_same_v<T, EventoItemDesaparecioSuelo>) {
            return { Opcode::ITEM_DESAPARECIO_SUELO,
                     MensajeItemDesaparecioSuelo{ e.x, e.y } };
        } else if constexpr (std::is_same_v<T, EventoActualizarInventario>) {
            return { Opcode::ACTUALIZAR_INVENTARIO,
                     MensajeActualizarInventario{ e.slots } };
        } else if constexpr (std::is_same_v<T, EventoActualizarEquipamiento>) {
            return { Opcode::ACTUALIZAR_EQUIPAMIENTO,
                     MensajeActualizarEquipamiento{
                         e.arma, e.baculo, e.defensa, e.casco, e.escudo } };
        } else if constexpr (std::is_same_v<T, EventoChat>) {
            return { Opcode::MENSAJE_CHAT,
                     MensajeChat{ e.nickOrigen, e.mensaje } };
        } else if constexpr (std::is_same_v<T, EventoClan>) {
            return { Opcode::MENSAJE_CLAN,
                     MensajeClan{ static_cast<TipoMensajeClan>(e.tipo), e.texto } };
        } else if constexpr (std::is_same_v<T, EventoResucitado>) {
            return { Opcode::RESUCITADO,
                     MensajeResucitado{ e.x, e.y } };
        } else if constexpr (std::is_same_v<T, EventoListaItems>) {
            return { Opcode::LISTA_ITEMS,
                     MensajeListaItems{ e.ids } };
        } else if constexpr (std::is_same_v<T, EventoErrorAccion>) {
            return { Opcode::ERROR_ACCION,
                     MensajeErrorAccion{ e.codigo } };
        } else {
            static_assert(!std::is_same_v<T, T>,
                          "Tipo de EventoJuego sin traduccion a MensajeServidor");
        }
    }, evento);
}

}  // namespace

MensajeSalida TraductorProtocolo::traducir(const EventoSalida& evento) {
    return { evento.tipoDestino, evento.idCliente, aMensajeServidor(evento.evento) };
}
