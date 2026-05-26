#ifndef MONITOR_CLIENTES_H
#define MONITOR_CLIENTES_H

#include <cstdint>
#include <map>
#include <mutex>
#include <unordered_map>

#include "../../common/protocolo/mensaje_servidor.h"
#include "../../common/thread/queue.h"
#include "mensaje_salida.h"

// de momento el monitor no es dueño de la colas --> estoy usando punteros crudos en la cola
// monitor se encargar de adminsitrar a los usuarios: les asigna un id al conectarse,
// asocia id con sus colas de salidas

class MonitorClientes {
private:
    // agrego un mapa que asocia id del usuario con su nombre
    std::unordered_map<uint16_t, std::string> nombresUsuariosConectados;
    std::unordered_map<uint16_t, std::string> nombresUsuariosDesconectados;

    // cambio nombre a colasClientes para que se mas explicito

    std::unordered_map<uint16_t, Queue<MensajeServidor>*> colasClientes;
    std::mutex mtx;

    uint16_t proximoID;

public:
    MonitorClientes();

    void agregarCliente(uint16_t idCliente, Queue<MensajeServidor>& colaSalida);
    void removerCliente(uint16_t idCliente);
    bool estaConectado(uint16_t idCliente);
    uint16_t idCliente(const std::string& nombre);

    uint16_t almacenarID();
    Queue<MensajeServidor>* getColasClientes(uint16_t idCliente);

    // almacenar los nombres de los usuarios con su id
    std::string nombreCliente(uint16_t idCliente);
    void setNombreCliente(uint16_t idCliente, const std::string& nombreCliente);

    void enviarA(uint16_t idCliente, const MensajeServidor& mensaje);
    void broadcast(const MensajeServidor& mensaje);
    void broadcastExcepto(uint16_t idClienteExcluido, const MensajeServidor& mensaje);

    // Con este método decidimos si el mensaje es individual o a todo el mundo
    void despachar(const MensajeSalida& mensajeSalida);

};

#endif

// uso de punteros crudos en las colas de salida
// lo mejor seria poner shared pointers, pero tengo que cambiar los parametros
// de los hilos enviador y receptor.
// la idea es que monitor maneja las colas y que este sea llamado en los hilos