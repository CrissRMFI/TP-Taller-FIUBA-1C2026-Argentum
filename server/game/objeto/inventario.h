#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <vector>
#include <map>

#include "item.h"
#include "equipamiento.h"

const uint8_t cantidadMaximaItems = 20; // Esta predeterminado, mover a un archivo de configuracion
class Inventario {
    public:

    bool agregarItem(uint16_t idItem);
    bool eliminarItem(uint16_t idItem);
    bool equiparItem(uint16_t idItem, TipoItem tipo);
    
    private:

    uint16_t slots[cantidadMaximaItems]; // Suponiendo 10 slots en el inventario, 0 significa slot vacío
    Equipamiento equipamiento;
    bool tieneItem(uint16_t idItem) const;
};

#endif