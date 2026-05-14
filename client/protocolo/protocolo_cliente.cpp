#include  "protocolo_cliente.h"

ProtocoloCliente::ProtocoloCliente(Socket&& skt) : Protocolo(std::move(skt)) {}

void ProtocoloCliente::enviarComando(const ComandoJugador& comando) {
    // envio del codigo de operacion
    enviarUnByte(comando.opcode);
    /*
     *  usar un switch case? que otra forma puede usarse?
     */
    switch (comando.opcode){
    case Opcode::MOVER:
        enviarComandoMover(comando.payload);
        break;
    case Opcode::ATACAR:
        enviarComandoAtacar(comando.payload);
        break;
    case Opcode::MEDITAR:
        enviarComandoMeditar(comando.payload);
    }
}

MensajeServidor ProtocoloCliente::recibirMensaje(){
    //trabajar de la misma forma que los comandos
}

void ProtocoloCliente::cerrarConexion(){
    this->skt.shutdown(2); // Shutdown de lectura y escritura
    this->skt.close();
    this->cerrado = true;
}

void ProtocoloCliente::enviarComandoMover(const ComandoMover& comando){
    enviarUnByte(comando.direccion)
}

void ProtocoloCliente::enviarComandoAtacar(const ComandoAtacar& comando){
    enviarDosByte(comando.idObjetivo)
}

void ProtocoloCliente::enviarComandoMeditar(const ComandoMeditar& comando){

}
