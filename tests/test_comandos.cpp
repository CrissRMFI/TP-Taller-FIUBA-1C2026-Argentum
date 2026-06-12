#include <gtest/gtest.h>

#include <string>

#include "../common/protocolo/comando_jugador.h"
#include "../common/protocolo/opcode.h"
#include "test_utils.h"

// Movimiento

TEST_F(ProtocoloFixture, EmpezarMoverNorte) {
    cliente->enviarComando({Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{0}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::EMPEZAR_MOVER);
    EXPECT_EQ(std::get<ComandoEmpezarMover>(cmd.payload).direccion, 0);
}

TEST_F(ProtocoloFixture, EmpezarMoverSur) {
    cliente->enviarComando({Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{1}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::EMPEZAR_MOVER);
    EXPECT_EQ(std::get<ComandoEmpezarMover>(cmd.payload).direccion, 1);
}

TEST_F(ProtocoloFixture, EmpezarMoverOeste) {
    cliente->enviarComando({Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{2}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::EMPEZAR_MOVER);
    EXPECT_EQ(std::get<ComandoEmpezarMover>(cmd.payload).direccion, 2);
}

TEST_F(ProtocoloFixture, EmpezarMoverEste) {
    cliente->enviarComando({Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{3}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::EMPEZAR_MOVER);
    EXPECT_EQ(std::get<ComandoEmpezarMover>(cmd.payload).direccion, 3);
}

TEST_F(ProtocoloFixture, DetenerMover) {
    cliente->enviarComando({Opcode::DETENER_MOVER, ComandoDetenerMover{}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::DETENER_MOVER);
}

// Combate

TEST_F(ProtocoloFixture, AtacarRoundTrip) {
    cliente->enviarComando({Opcode::ATACAR, ComandoAtacar{42}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::ATACAR);
    EXPECT_EQ(std::get<ComandoAtacar>(cmd.payload).idObjetivo, 42);
}

TEST_F(ProtocoloFixture, AtacarIdMinimo) {
    cliente->enviarComando({Opcode::ATACAR, ComandoAtacar{0}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(std::get<ComandoAtacar>(cmd.payload).idObjetivo, 0);
}

TEST_F(ProtocoloFixture, AtacarIdMaximo) {
    cliente->enviarComando({Opcode::ATACAR, ComandoAtacar{65535}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(std::get<ComandoAtacar>(cmd.payload).idObjetivo, 65535);
}

TEST_F(ProtocoloFixture, Curar) {
    cliente->enviarComando({Opcode::CURAR, ComandoCurar{100}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::CURAR);
    EXPECT_EQ(std::get<ComandoCurar>(cmd.payload).idSacerdote, 100);
}

// Comandos ssin payload

TEST_F(ProtocoloFixture, Meditar) {
    cliente->enviarComando({Opcode::MEDITAR, ComandoMeditar{}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::MEDITAR);
}

TEST_F(ProtocoloFixture, Resucitar) {
    cliente->enviarComando({Opcode::RESUCITAR, ComandoResucitar{}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::RESUCITAR);
}

TEST_F(ProtocoloFixture, Tomar) {
    cliente->enviarComando({Opcode::TOMAR, ComandoTomar{}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::TOMAR);
}

TEST_F(ProtocoloFixture, RevisarClan) {
    cliente->enviarComando({Opcode::REVISAR_CLAN, ComandoRevisarClan{}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::REVISAR_CLAN);
}

TEST_F(ProtocoloFixture, DejarClan) {
    cliente->enviarComando({Opcode::DEJAR_CLAN, ComandoDejarClan{}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::DEJAR_CLAN);
}

// Inventario

TEST_F(ProtocoloFixture, Tirar) {
    cliente->enviarComando({Opcode::TIRAR, ComandoTirar{5}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::TIRAR);
    EXPECT_EQ(std::get<ComandoTirar>(cmd.payload).indiceItem, 5);
}

TEST_F(ProtocoloFixture, TirarIndiceCero) {
    cliente->enviarComando({Opcode::TIRAR, ComandoTirar{0}});
    EXPECT_EQ(std::get<ComandoTirar>(servidor->recibirComando().payload).indiceItem, 0);
}

TEST_F(ProtocoloFixture, TirarIndiceMaximoUint8) {
    cliente->enviarComando({Opcode::TIRAR, ComandoTirar{255}});
    EXPECT_EQ(std::get<ComandoTirar>(servidor->recibirComando().payload).indiceItem, 255);
}

TEST_F(ProtocoloFixture, Equipar) {
    cliente->enviarComando({Opcode::EQUIPAR, ComandoEquipar{3}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::EQUIPAR);
    EXPECT_EQ(std::get<ComandoEquipar>(cmd.payload).indiceItem, 3);
}

// Comercio y banco

TEST_F(ProtocoloFixture, Comprar) {
    cliente->enviarComando({Opcode::COMPRAR, ComandoComprar{100, 200}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::COMPRAR);
    auto p = std::get<ComandoComprar>(cmd.payload);
    EXPECT_EQ(p.idItem, 100);
    EXPECT_EQ(p.idNPC, 200);
}

TEST_F(ProtocoloFixture, Vender) {
    cliente->enviarComando({Opcode::VENDER, ComandoVender{2, 50}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::VENDER);
    auto p = std::get<ComandoVender>(cmd.payload);
    EXPECT_EQ(p.indiceItem, 2);
    EXPECT_EQ(p.idNPC, 50);
}

TEST_F(ProtocoloFixture, DepositarItem) {
    cliente->enviarComando({Opcode::DEPOSITAR_ITEM, ComandoDepositarItem{4, 77}});
    auto p = std::get<ComandoDepositarItem>(servidor->recibirComando().payload);
    EXPECT_EQ(p.indiceItem, 4);
    EXPECT_EQ(p.idBanquero, 77);
}

TEST_F(ProtocoloFixture, DepositarOro) {
    cliente->enviarComando({Opcode::DEPOSITAR_ORO, ComandoDepositarOro{1000, 77}});
    auto p = std::get<ComandoDepositarOro>(servidor->recibirComando().payload);
    EXPECT_EQ(p.monto, 1000u);
    EXPECT_EQ(p.idBanquero, 77);
}

TEST_F(ProtocoloFixture, DepositarOroMontoMaximo) {
    cliente->enviarComando({Opcode::DEPOSITAR_ORO, ComandoDepositarOro{0xFFFFFFFF, 77}});
    auto p = std::get<ComandoDepositarOro>(servidor->recibirComando().payload);
    EXPECT_EQ(p.monto, 0xFFFFFFFFu);
}

TEST_F(ProtocoloFixture, RetirarItem) {
    cliente->enviarComando({Opcode::RETIRAR_ITEM, ComandoRetirarItem{12, 77}});
    auto p = std::get<ComandoRetirarItem>(servidor->recibirComando().payload);
    EXPECT_EQ(p.idItem, 12);
    EXPECT_EQ(p.idBanquero, 77);
}

TEST_F(ProtocoloFixture, RetirarOro) {
    cliente->enviarComando({Opcode::RETIRAR_ORO, ComandoRetirarOro{500, 77}});
    auto p = std::get<ComandoRetirarOro>(servidor->recibirComando().payload);
    EXPECT_EQ(p.monto, 500u);
    EXPECT_EQ(p.idBanquero, 77);
}

TEST_F(ProtocoloFixture, Listar) {
    cliente->enviarComando({Opcode::LISTAR, ComandoListar{99}});
    EXPECT_EQ(std::get<ComandoListar>(servidor->recibirComando().payload).idNPC, 99);
}

// Chat

TEST_F(ProtocoloFixture, ChatGlobalMensajeCorto) {
    cliente->enviarComando({Opcode::CHAT_GLOBAL, ComandoChatGlobal{"hola"}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::CHAT_GLOBAL);
    EXPECT_EQ(std::get<ComandoChatGlobal>(cmd.payload).mensaje, "hola");
}

TEST_F(ProtocoloFixture, ChatGlobalMensajeVacio) {
    cliente->enviarComando({Opcode::CHAT_GLOBAL, ComandoChatGlobal{""}});
    EXPECT_EQ(std::get<ComandoChatGlobal>(servidor->recibirComando().payload).mensaje, "");
}

TEST_F(ProtocoloFixture, ChatGlobalConCaracteresEspeciales) {
    std::string msg = "che ¿estás ahí? @#$%";
    cliente->enviarComando({Opcode::CHAT_GLOBAL, ComandoChatGlobal{msg}});
    EXPECT_EQ(std::get<ComandoChatGlobal>(servidor->recibirComando().payload).mensaje, msg);
}

TEST_F(ProtocoloFixture, ChatPrivadoRoundTrip) {
    cliente->enviarComando({Opcode::CHAT_PRIVADO, ComandoChatPrivado{"juan", "hola juan"}});
    auto cmd = servidor->recibirComando();
    auto p = std::get<ComandoChatPrivado>(cmd.payload);
    EXPECT_EQ(p.nickDestino, "juan");
    EXPECT_EQ(p.mensaje, "hola juan");
}

// Clanes

TEST_F(ProtocoloFixture, FundarClan) {
    cliente->enviarComando({Opcode::FUNDAR_CLAN, ComandoFundarClan{"LosCapos"}});
    EXPECT_EQ(std::get<ComandoFundarClan>(servidor->recibirComando().payload).nombreClan, "LosCapos");
}

TEST_F(ProtocoloFixture, UnirseClan) {
    cliente->enviarComando({Opcode::UNIRSE_CLAN, ComandoUnirseClan{"LosCapos"}});
    EXPECT_EQ(std::get<ComandoUnirseClan>(servidor->recibirComando().payload).nombreClan, "LosCapos");
}

TEST_F(ProtocoloFixture, ClanAceptar) {
    cliente->enviarComando({Opcode::CLAN_ACEPTAR, ComandoGestionMiembreClan{"pepe"}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::CLAN_ACEPTAR);
    EXPECT_EQ(std::get<ComandoGestionMiembreClan>(cmd.payload).nick, "pepe");
}

TEST_F(ProtocoloFixture, ClanRechazar) {
    cliente->enviarComando({Opcode::CLAN_RECHAZAR, ComandoGestionMiembreClan{"pepe"}});
    auto cmd = servidor->recibirComando();
    EXPECT_EQ(cmd.opcode, Opcode::CLAN_RECHAZAR);
    EXPECT_EQ(std::get<ComandoGestionMiembreClan>(cmd.payload).nick, "pepe");
}

TEST_F(ProtocoloFixture, ClanBan) {
    cliente->enviarComando({Opcode::CLAN_BAN, ComandoGestionMiembreClan{"malo"}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::CLAN_BAN);
}

TEST_F(ProtocoloFixture, ClanKick) {
    cliente->enviarComando({Opcode::CLAN_KICK, ComandoGestionMiembreClan{"malo"}});
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::CLAN_KICK);
}

// Varios comandos en el mismo socket

TEST_F(ProtocoloFixture, SecuenciaDeComandosPreservaOrden) {
    cliente->enviarComando({Opcode::EMPEZAR_MOVER, ComandoEmpezarMover{0}});
    cliente->enviarComando({Opcode::ATACAR, ComandoAtacar{99}});
    cliente->enviarComando({Opcode::MEDITAR, ComandoMeditar{}});
    cliente->enviarComando({Opcode::DETENER_MOVER, ComandoDetenerMover{}});

    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::EMPEZAR_MOVER);
    EXPECT_EQ(std::get<ComandoAtacar>(servidor->recibirComando().payload).idObjetivo, 99);
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::MEDITAR);
    EXPECT_EQ(servidor->recibirComando().opcode, Opcode::DETENER_MOVER);
}