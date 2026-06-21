#ifndef BANQUERO_H
#define BANQUERO_H

#include "npc.h"

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

/*
Documentación:
Banquero tiene el atributo cuentas. Ese atributo es un map, clave -> valor
clave -> (uint16_t)
valor -> std::pair<uint32_t, std::vector<uint16_t>>

Por qué usamos estos tipos de datos ?
Para la clave, uint16_t porque con eso vamos a identificar el propietario de la cuenta
Para el valor, representa un par de datos asociados, el uint32_t es para el oro en el banco
y el vector<uint16_t> es para guardar todos los id de los items depositados.
*/


class Banquero : public Npc {
private:
    std::map<uint16_t, std::pair<uint32_t, std::vector<uint16_t>>> cuentas;

public:
    Banquero(uint16_t id, Posicion posicion);

    std::pair<uint32_t, std::vector<uint16_t>> listarItemsDisponibles(uint16_t idJugador);

    bool depositarOro(uint16_t idJugador, uint32_t cantidad);

    bool retirarOro(uint16_t idJugador, uint32_t cantidad);

    bool depositarItem(uint16_t idJugador, uint16_t idItem);

    bool retirarItem(uint16_t idJugador, uint16_t idItem);

    bool tieneItem(uint16_t idJugador, uint16_t idItem) const;
};

#endif
