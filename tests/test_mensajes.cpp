#include <gtest/gtest.h>

#include <string>

#include "../common/protocolo/mensaje_servidor.h"
#include "../common/protocolo/opcode.h"
#include "test_utils.h"

// Esstado del personaje y posicion

TEST_F(ProtocoloFixture, EstadoPersonajeRoundTrip) {
    MensajeEstadoPersonaje m{100, 150, 50, 80, 1000, 5, 250, 0};
    servidor->enviarMensaje({Opcode::ESTADO_PERSONAJE, m});
    auto msg = cliente->recibirMensaje();
    EXPECT_EQ(msg.opcode, Opcode::ESTADO_PERSONAJE);
    auto p = std::get<MensajeEstadoPersonaje>(msg.payload);
    EXPECT_EQ(p.vidaActual, 100);
    EXPECT_EQ(p.vidaMax, 150);
    EXPECT_EQ(p.manaActual, 50);
    EXPECT_EQ(p.manaMax, 80);
    EXPECT_EQ(p.oro, 1000u);
    EXPECT_EQ(p.nivel, 5);
    EXPECT_EQ(p.experiencia, 250u);
    EXPECT_EQ(p.estado, 0);
}

TEST_F(ProtocoloFixture, EstadoPersonajeValoresMaximos) {
    MensajeEstadoPersonaje m{65535, 65535, 65535, 65535, 0xFFFFFFFF, 255, 0xFFFFFFFF, 0};
    servidor->enviarMensaje({Opcode::ESTADO_PERSONAJE, m});
    auto p = std::get<MensajeEstadoPersonaje>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.oro, 0xFFFFFFFFu);
    EXPECT_EQ(p.experiencia, 0xFFFFFFFFu);
}

TEST_F(ProtocoloFixture, PosicionEntidadRoundTrip) {
    MensajePosicionEntidad m{7, 50, 50, 0, 0, 0, 0, 0, 0, 0};
    servidor->enviarMensaje({Opcode::POSICION_ENTIDAD, m});
    auto p = std::get<MensajePosicionEntidad>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.id, 7);
    EXPECT_EQ(p.x, 50);
    EXPECT_EQ(p.y, 50);
    EXPECT_EQ(p.tipo, 0);
    EXPECT_EQ(p.estado, 0);
}

TEST_F(ProtocoloFixture, EntidadDesaparecio) {
    servidor->enviarMensaje({Opcode::ENTIDAD_DESAPARECIO, MensajeEntidadDesaparecio{42}});
    EXPECT_EQ(std::get<MensajeEntidadDesaparecio>(cliente->recibirMensaje().payload).id, 42);
}

// Combate

TEST_F(ProtocoloFixture, DanoRecibido) {
    servidor->enviarMensaje({Opcode::DANIO_RECIBIDO, MensajeDanoRecibido{15, 99}});
    auto p = std::get<MensajeDanoRecibido>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.cantidad, 15);
    EXPECT_EQ(p.idAtacante, 99);
}

TEST_F(ProtocoloFixture, DanoProducido) {
    servidor->enviarMensaje({Opcode::DANIO_PRODUCIDO, MensajeDanoProducido{20, 50, 0, 0}});
    auto p = std::get<MensajeDanoProducido>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.cantidad, 20);
    EXPECT_EQ(p.idObjetivo, 50);
}

TEST_F(ProtocoloFixture, Esquive) {
    servidor->enviarMensaje({Opcode::ESQUIVE, MensajeEsquive{77, 0}});
    auto p = std::get<MensajeEsquive>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.idEntidad, 77);
    EXPECT_EQ(p.esquivador, 0);
}

TEST_F(ProtocoloFixture, MuerteEntidad) {
    servidor->enviarMensaje({Opcode::MUERTE_ENTIDAD, MensajeMuerteEntidad{33}});
    EXPECT_EQ(std::get<MensajeMuerteEntidad>(cliente->recibirMensaje().payload).id, 33);
}

TEST_F(ProtocoloFixture, Resucitado) {
    servidor->enviarMensaje({Opcode::RESUCITADO, MensajeResucitado{30, 40}});
    auto p = std::get<MensajeResucitado>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.x, 30);
    EXPECT_EQ(p.y, 40);
}

// Items del mundo

TEST_F(ProtocoloFixture, ItemEnSuelo) {
    servidor->enviarMensaje({Opcode::ITEM_EN_SUELO, MensajeItemEnSuelo{5, 10, 20}});
    auto p = std::get<MensajeItemEnSuelo>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.idItem, 5);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

TEST_F(ProtocoloFixture, ItemDesaparecioSuelo) {
    servidor->enviarMensaje({Opcode::ITEM_DESAPARECIO_SUELO, MensajeItemDesaparecioSuelo{10, 20}});
    auto p = std::get<MensajeItemDesaparecioSuelo>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

// Inventario y eqipo

TEST_F(ProtocoloFixture, ActualizarInventarioVacio) {
    servidor->enviarMensaje({Opcode::ACTUALIZAR_INVENTARIO, MensajeActualizarInventario{{}}});
    EXPECT_TRUE(std::get<MensajeActualizarInventario>(cliente->recibirMensaje().payload).slots_.empty());
}

TEST_F(ProtocoloFixture, ActualizarInventarioConItems) {
    MensajeActualizarInventario m;
    m.slots_ = {100, 200, 300, 400};
    servidor->enviarMensaje({Opcode::ACTUALIZAR_INVENTARIO, m});
    auto p = std::get<MensajeActualizarInventario>(cliente->recibirMensaje().payload);
    ASSERT_EQ(p.slots_.size(), 4u);
    EXPECT_EQ(p.slots_[0], 100);
    EXPECT_EQ(p.slots_[3], 400);
}

TEST_F(ProtocoloFixture, ActualizarInventarioMaximo) {
    MensajeActualizarInventario m;
    for (uint16_t i = 0; i < 255; ++i) m.slots_.push_back(i);
    servidor->enviarMensaje({Opcode::ACTUALIZAR_INVENTARIO, m});
    auto p = std::get<MensajeActualizarInventario>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.slots_.size(), 255u);
    EXPECT_EQ(p.slots_[254], 254);
}

TEST_F(ProtocoloFixture, ActualizarEquipamiento) {
    MensajeActualizarEquipamiento m{1, 2, 3, 4, 5};
    servidor->enviarMensaje({Opcode::ACTUALIZAR_EQUIPAMIENTO, m});
    auto p = std::get<MensajeActualizarEquipamiento>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.arma, 1);
    EXPECT_EQ(p.baculo, 2);
    EXPECT_EQ(p.defensa, 3);
    EXPECT_EQ(p.casco, 4);
    EXPECT_EQ(p.escudo, 5);
}

TEST_F(ProtocoloFixture, ActualizarEquipamientoTodoVacio) {
    MensajeActualizarEquipamiento m{0, 0, 0, 0, 0};
    servidor->enviarMensaje({Opcode::ACTUALIZAR_EQUIPAMIENTO, m});
    auto p = std::get<MensajeActualizarEquipamiento>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.arma, 0);
    EXPECT_EQ(p.escudo, 0);
}

// Chat

TEST_F(ProtocoloFixture, MensajeChat) {
    servidor->enviarMensaje({Opcode::MENSAJE_CHAT, MensajeChat{"pepe", "hola a todos"}});
    auto p = std::get<MensajeChat>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.nickOrigen, "pepe");
    EXPECT_EQ(p.mensaje, "hola a todos");
}

TEST_F(ProtocoloFixture, MensajeChatMensajeVacio) {
    servidor->enviarMensaje({Opcode::MENSAJE_CHAT, MensajeChat{"pepe", ""}});
    EXPECT_EQ(std::get<MensajeChat>(cliente->recibirMensaje().payload).mensaje, "");
}

TEST_F(ProtocoloFixture, MensajeClan) {
    servidor->enviarMensaje({Opcode::MENSAJE_CLAN, MensajeClan{TipoMensajeClan::Fundado, "LosCapos"}});
    auto p = std::get<MensajeClan>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.tipo, TipoMensajeClan::Fundado);
    EXPECT_EQ(p.texto, "LosCapos");
}

// Listas

TEST_F(ProtocoloFixture, ListaItemsVacia) {
    servidor->enviarMensaje({Opcode::LISTA_ITEMS, MensajeListaItems{{}}});
    EXPECT_TRUE(std::get<MensajeListaItems>(cliente->recibirMensaje().payload).ids.empty());
}

TEST_F(ProtocoloFixture, ListaItemsConElementos) {
    MensajeListaItems m;
    m.ids = {10, 20, 30};
    servidor->enviarMensaje({Opcode::LISTA_ITEMS, m});
    auto p = std::get<MensajeListaItems>(cliente->recibirMensaje().payload);
    ASSERT_EQ(p.ids.size(), 3u);
    EXPECT_EQ(p.ids[0], 10);
    EXPECT_EQ(p.ids[2], 30);
}

// Secuencia

TEST_F(ProtocoloFixture, SecuenciaDeMensajesPreservaOrden) {
    servidor->enviarMensaje({Opcode::ESTADO_PERSONAJE, MensajeEstadoPersonaje{100, 100, 50, 50, 0, 1, 0, 0}});
    servidor->enviarMensaje({Opcode::POSICION_ENTIDAD, MensajePosicionEntidad{1, 50, 50, 0, 0, 0, 0, 0, 0, 0}});
    servidor->enviarMensaje({Opcode::DANIO_RECIBIDO, MensajeDanoRecibido{10, 99}});

    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::ESTADO_PERSONAJE);
    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::POSICION_ENTIDAD);
    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::DANIO_RECIBIDO);
}