#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <cstdint>
#include <vector>

#include "item.h"
#include "equipamiento.h"

const uint8_t cantidadMaximaItems = 20;

class Inventario {
public:
    Inventario();

    bool agregarItem(uint16_t idItem);
    bool eliminarItem(uint16_t idItem);
    bool tieneItem(uint16_t idItem) const;

    // Vacia todos los slots y retorna los IDs que habia (usado al morir)
    std::vector<uint16_t> vaciar();

    // Equipa un item: lo saca del inventario, lo pone en el slot correcto
    // y devuelve al inventario los items desplazados.
    // Para Defensa usar equiparPieza() que requiere saber el sub-slot.
    bool equiparItem(uint16_t idItem, TipoItem tipo);
    bool equiparPieza(uint16_t idItem, TipoDefensa slot);

    uint16_t getArmaEquipada()    const;
    uint16_t getBaculoEquipado()  const;
    uint16_t getDefensaEquipada() const;
    uint16_t getCascoEquipado()   const;
    uint16_t getEscudoEquipado()  const;

private:
    uint16_t slots[cantidadMaximaItems];
    Equipamiento equipamiento;
};

#endif
