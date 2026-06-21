#include "traductor_protocolo.h"

#include <stdexcept>
#include <variant>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/protocolo/opcode.h"

static MensajeServidor aMensajeServidor(const EventoJuego& evento) {
    if (auto* e = std::get_if<EventoEstadoPersonaje>(&evento)) {
        return { Opcode::ESTADO_PERSONAJE,
                 MensajeEstadoPersonaje{
                     e->vidaActual, e->vidaMax,
                     e->manaActual, e->manaMax,
                     e->oro, e->nivel, e->experiencia,
                     static_cast<uint8_t>(e->estado),
                     e->raza, e->clase, e->expSiguienteNivel,
                     e->tiempoResurreccionMs } };
    }
    if (auto* e = std::get_if<EventoPosicionEntidad>(&evento)) {
        return { Opcode::POSICION_ENTIDAD,
                 MensajePosicionEntidad{ e->id, e->x, e->y, e->tipo, e->estado, e->cabeza,
                                         e->cuerpo, e->arma, e->escudo, e->casco, e->mapaId } };
    }
    if (auto* e = std::get_if<EventoEntidadDesaparecio>(&evento)) {
        return { Opcode::ENTIDAD_DESAPARECIO,
                 MensajeEntidadDesaparecio{ e->id } };
    }
    if (auto* e = std::get_if<EventoDanioRecibido>(&evento)) {
        return { Opcode::DANIO_RECIBIDO,
                 MensajeDanoRecibido{ e->cantidad, e->idAtacante,
                                      static_cast<uint8_t>(e->esCritico ? 1 : 0) } };
    }
    if (auto* e = std::get_if<EventoDanioProducido>(&evento)) {
        return { Opcode::DANIO_PRODUCIDO,
                 MensajeDanoProducido{ e->cantidad, e->idObjetivo, e->tipoGolpe,
                                       static_cast<uint8_t>(e->esCritico ? 1 : 0) } };
    }
    if (auto* e = std::get_if<EventoEsquive>(&evento)) {
        return { Opcode::ESQUIVE,
                 MensajeEsquive{ e->idEntidad, e->esquivador } };
    }
    if (auto* e = std::get_if<EventoMuerteEntidad>(&evento)) {
        return { Opcode::MUERTE_ENTIDAD,
                 MensajeMuerteEntidad{ e->id } };
    }
    if (auto* e = std::get_if<EventoItemEnSuelo>(&evento)) {
        return { Opcode::ITEM_EN_SUELO,
                 MensajeItemEnSuelo{ e->idItem, e->x, e->y } };
    }
    if (auto* e = std::get_if<EventoItemDesaparecioSuelo>(&evento)) {
        return { Opcode::ITEM_DESAPARECIO_SUELO,
                 MensajeItemDesaparecioSuelo{ e->x, e->y } };
    }
    if (auto* e = std::get_if<EventoOroEnSuelo>(&evento)) {
        return { Opcode::ORO_EN_SUELO,
                 MensajeOroEnSuelo{ e->cantidad, e->x, e->y } };
    }
    if (auto* e = std::get_if<EventoOroDesaparecioSuelo>(&evento)) {
        return { Opcode::ORO_DESAPARECIO_SUELO,
                 MensajeOroDesaparecioSuelo{ e->x, e->y } };
    }
    if (auto* e = std::get_if<EventoActualizarInventario>(&evento)) {
        return { Opcode::ACTUALIZAR_INVENTARIO,
                 MensajeActualizarInventario{ e->slots } };
    }
    if (auto* e = std::get_if<EventoActualizarEquipamiento>(&evento)) {
        return { Opcode::ACTUALIZAR_EQUIPAMIENTO,
                 MensajeActualizarEquipamiento{
                     e->arma, e->baculo, e->defensa, e->casco, e->escudo } };
    }
    if (auto* e = std::get_if<EventoChat>(&evento)) {
        return { Opcode::MENSAJE_CHAT,
                 MensajeChat{ e->nickOrigen, e->mensaje } };
    }
    if (auto* e = std::get_if<EventoResucitado>(&evento)) {
        return { Opcode::RESUCITADO,
                 MensajeResucitado{ e->x, e->y } };
    }
    if (auto* e = std::get_if<EventoListaItems>(&evento)) {
        return { Opcode::LISTA_ITEMS,
                 MensajeListaItems{ e->ids } };
    }
    if (auto* e = std::get_if<EventoContenidoBanco>(&evento)) {
        return { Opcode::CONTENIDO_BANCO,
                 MensajeContenidoBanco{ e->items, e->oroBanco } };
    }
    if (auto* e = std::get_if<EventoListaHechizos>(&evento)) {
        return { Opcode::LISTA_HECHIZOS, MensajeListaHechizos{ e->ids } };
    }
    if (auto* e = std::get_if<EventoFxHechizo>(&evento)) {
        return { Opcode::FX_HECHIZO, MensajeFxHechizo{ e->idHechizo, e->idObjetivo } };
    }
    if (auto* e = std::get_if<EventoCambioMapa>(&evento)) {
        return { Opcode::CAMBIO_MAPA, MensajeCambioMapa{ e->mapaId } };
    }
    if (auto* e = std::get_if<EventoProyectil>(&evento)) {
        return { Opcode::PROYECTIL, MensajeProyectil{ e->idOrigen, e->idDestino } };
    }
    if (auto* e = std::get_if<EventoErrorAccion>(&evento)) {
        return { Opcode::ERROR_ACCION,
                 MensajeErrorAccion{ e->codigo } };
    }

    throw std::runtime_error("Tipo de EventoJuego sin traduccion a MensajeServidor");
}

MensajeSalida TraductorProtocolo::traducir(const EventoSalida& evento) {
    return { evento.tipoDestino, evento.idCliente, aMensajeServidor(evento.evento) };
}
