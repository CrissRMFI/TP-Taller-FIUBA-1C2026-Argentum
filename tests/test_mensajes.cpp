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
    MensajePosicionEntidad m{7, 50, 50, 0, 0, 0, 0, 0, 0, 0, 3};
    servidor->enviarMensaje({Opcode::POSICION_ENTIDAD, m});
    auto p = std::get<MensajePosicionEntidad>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.id, 7);
    EXPECT_EQ(p.x, 50);
    EXPECT_EQ(p.y, 50);
    EXPECT_EQ(p.tipo, 0);
    EXPECT_EQ(p.estado, 0);
    EXPECT_EQ(p.mapaId, 3);
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

TEST_F(ProtocoloFixture, Proyectil) {
    servidor->enviarMensaje({Opcode::PROYECTIL, MensajeProyectil{1, 2}});
    auto p = std::get<MensajeProyectil>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.idOrigen, 1);
    EXPECT_EQ(p.idDestino, 2);
}

TEST_F(ProtocoloFixture, ProyectilIdsMaximos) {
    servidor->enviarMensaje({Opcode::PROYECTIL, MensajeProyectil{65535, 65535}});
    auto p = std::get<MensajeProyectil>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.idOrigen, 65535);
    EXPECT_EQ(p.idDestino, 65535);
}

TEST_F(ProtocoloFixture, CambioMapa) {
    servidor->enviarMensaje({Opcode::CAMBIO_MAPA, MensajeCambioMapa{1}});
    auto p = std::get<MensajeCambioMapa>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.mapaId, 1);
}

TEST_F(ProtocoloFixture, CambioMapaIdMaximo) {
    servidor->enviarMensaje({Opcode::CAMBIO_MAPA, MensajeCambioMapa{65535}});
    auto p = std::get<MensajeCambioMapa>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.mapaId, 65535);
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

TEST_F(ProtocoloFixture, OroEnSuelo) {
    servidor->enviarMensaje({Opcode::ORO_EN_SUELO, MensajeOroEnSuelo{500, 10, 20}});
    auto p = std::get<MensajeOroEnSuelo>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.cantidad, 500u);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

TEST_F(ProtocoloFixture, OroEnSueloCantidadMaxima) {
    servidor->enviarMensaje({Opcode::ORO_EN_SUELO, MensajeOroEnSuelo{0xFFFFFFFF, 0, 0}});
    EXPECT_EQ(std::get<MensajeOroEnSuelo>(cliente->recibirMensaje().payload).cantidad, 0xFFFFFFFFu);
}

TEST_F(ProtocoloFixture, OroDesaparecioSuelo) {
    servidor->enviarMensaje({Opcode::ORO_DESAPARECIO_SUELO, MensajeOroDesaparecioSuelo{15, 25}});
    auto p = std::get<MensajeOroDesaparecioSuelo>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.x, 15);
    EXPECT_EQ(p.y, 25);
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

// Banco

TEST_F(ProtocoloFixture, ContenidoBancoVacio) {
    servidor->enviarMensaje({Opcode::CONTENIDO_BANCO, MensajeContenidoBanco{{}, 0}});
    auto p = std::get<MensajeContenidoBanco>(cliente->recibirMensaje().payload);
    EXPECT_TRUE(p.items.empty());
    EXPECT_EQ(p.oroBanco, 0u);
}

TEST_F(ProtocoloFixture, ContenidoBancoConItems) {
    MensajeContenidoBanco m;
    m.items = {100, 200, 300};
    m.oroBanco = 5000;
    servidor->enviarMensaje({Opcode::CONTENIDO_BANCO, m});
    auto p = std::get<MensajeContenidoBanco>(cliente->recibirMensaje().payload);
    ASSERT_EQ(p.items.size(), 3u);
    EXPECT_EQ(p.items[0], 100);
    EXPECT_EQ(p.items[2], 300);
    EXPECT_EQ(p.oroBanco, 5000u);
}

TEST_F(ProtocoloFixture, ContenidoBancoOroMaximo) {
    servidor->enviarMensaje({Opcode::CONTENIDO_BANCO, MensajeContenidoBanco{{}, 0xFFFFFFFF}});
    EXPECT_EQ(std::get<MensajeContenidoBanco>(cliente->recibirMensaje().payload).oroBanco, 0xFFFFFFFFu);
}

// Hechizos

TEST_F(ProtocoloFixture, ListaHechizosVacia) {
    servidor->enviarMensaje({Opcode::LISTA_HECHIZOS, MensajeListaHechizos{{}}});
    EXPECT_TRUE(std::get<MensajeListaHechizos>(cliente->recibirMensaje().payload).ids.empty());
}

TEST_F(ProtocoloFixture, ListaHechizosConElementos) {
    MensajeListaHechizos m;
    m.ids = {1, 5, 10, 42};
    servidor->enviarMensaje({Opcode::LISTA_HECHIZOS, m});
    auto p = std::get<MensajeListaHechizos>(cliente->recibirMensaje().payload);
    ASSERT_EQ(p.ids.size(), 4u);
    EXPECT_EQ(p.ids[0], 1);
    EXPECT_EQ(p.ids[3], 42);
}

TEST_F(ProtocoloFixture, FxHechizo) {
    servidor->enviarMensaje({Opcode::FX_HECHIZO, MensajeFxHechizo{7, 99}});
    auto p = std::get<MensajeFxHechizo>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.idHechizo, 7);
    EXPECT_EQ(p.idObjetivo, 99);
}

TEST_F(ProtocoloFixture, FxHechizoIdsMaximos) {
    servidor->enviarMensaje({Opcode::FX_HECHIZO, MensajeFxHechizo{65535, 65535}});
    auto p = std::get<MensajeFxHechizo>(cliente->recibirMensaje().payload);
    EXPECT_EQ(p.idHechizo, 65535);
    EXPECT_EQ(p.idObjetivo, 65535);
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

// Estado de usuario

TEST_F(ProtocoloFixture, EstadoUsuarioSinError) {
    servidor->enviarEstadoUsuario(MensajeEstadoUsuario{1, "pepe", ErrorUsuario::Ninguno});
    auto msg = cliente->recibirEstadoUsuario();
    EXPECT_EQ(msg.opcode, Opcode::ESTADO_USUARIO);
    auto p = std::get<MensajeEstadoUsuario>(msg.payload);
    EXPECT_EQ(p.id, 1);
    EXPECT_EQ(p.nick, "pepe");
    EXPECT_EQ(p.error, ErrorUsuario::Ninguno);
}

TEST_F(ProtocoloFixture, EstadoUsuarioNickNoEncontrado) {
    servidor->enviarEstadoUsuario(MensajeEstadoUsuario{0, "fantasma", ErrorUsuario::NombreUsuarioNoEncontrado});
    auto p = std::get<MensajeEstadoUsuario>(cliente->recibirEstadoUsuario().payload);
    EXPECT_EQ(p.error, ErrorUsuario::NombreUsuarioNoEncontrado);
}

TEST_F(ProtocoloFixture, EstadoUsuarioNickYaExistente) {
    servidor->enviarEstadoUsuario(MensajeEstadoUsuario{2, "juan", ErrorUsuario::NickYaExistente});
    auto p = std::get<MensajeEstadoUsuario>(cliente->recibirEstadoUsuario().payload);
    EXPECT_EQ(p.error, ErrorUsuario::NickYaExistente);
}

TEST_F(ProtocoloFixture, EstadoUsuarioUsuarioYaConectado) {
    servidor->enviarEstadoUsuario(MensajeEstadoUsuario{3, "ya_logueado", ErrorUsuario::UsuarioYaConectado});
    auto p = std::get<MensajeEstadoUsuario>(cliente->recibirEstadoUsuario().payload);
    EXPECT_EQ(p.error, ErrorUsuario::UsuarioYaConectado);
}

// Secuencia

TEST_F(ProtocoloFixture, SecuenciaDeMensajesPreservaOrden) {
    servidor->enviarMensaje({Opcode::ESTADO_PERSONAJE, MensajeEstadoPersonaje{100, 100, 50, 50, 0, 1, 0, 0}});
    servidor->enviarMensaje({Opcode::POSICION_ENTIDAD, MensajePosicionEntidad{1, 50, 50, 0, 0, 0, 0, 0, 0, 0}});
    servidor->enviarMensaje({Opcode::DANIO_RECIBIDO, MensajeDanoRecibido{10, 99}});
    servidor->enviarMensaje({Opcode::FX_HECHIZO, MensajeFxHechizo{5, 99}});
    servidor->enviarMensaje({Opcode::PROYECTIL, MensajeProyectil{1, 99}});

    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::ESTADO_PERSONAJE);
    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::POSICION_ENTIDAD);
    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::DANIO_RECIBIDO);
    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::FX_HECHIZO);
    EXPECT_EQ(cliente->recibirMensaje().opcode, Opcode::PROYECTIL);
}