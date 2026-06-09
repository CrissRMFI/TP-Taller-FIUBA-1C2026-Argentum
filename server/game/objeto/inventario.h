#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "equipamiento.h"
#include "item.h"

class Inventario {
public:
    explicit Inventario(uint8_t cantidadMaximaItems);

    bool agregarItem(uint16_t idItem);
    bool eliminarItem(uint16_t idItem);
    bool eliminarSlot(uint8_t indice);
    uint16_t quitarDeSlot(uint8_t indice);
    void agregarItemEnSlot(uint16_t idItem, uint8_t indice);

    bool tieneItem(uint16_t idItem) const;
    bool tieneEspacioLibre() const;
    uint16_t getIdEnSlot(uint8_t indice) const;
    std::vector<uint16_t> getSlots() const;

    // Vacia todos los slots y retorna los IDs que habia, usado al morir.
    std::vector<uint16_t> vaciar();

    // Equipa un item: lo saca del inventario, lo pone en el slot correcto y devuelve al inventario los items desplazados.
    // Para Defensa usar equiparPieza(), que requiere saber el sub-slot.
    bool equiparItem(uint16_t idItem, TipoItem tipo);
    bool equiparPieza(uint16_t idItem, TipoDefensa slot);
    bool equiparSlot(uint8_t indice, TipoItem tipo);
    bool equiparPiezaSlot(uint8_t indice, TipoDefensa slot);

    uint16_t getArmaEquipada() const;
    uint16_t getBaculoEquipado() const;
    uint16_t getDefensaEquipada() const;
    uint16_t getCascoEquipado() const;
    uint16_t getEscudoEquipado() const;

    void restaurar(const std::vector<uint16_t>& slotsNuevos,
                   uint16_t arma,
                   uint16_t baculo,
                   uint16_t defensa,
                   uint16_t casco,
                   uint16_t escudo);

private:
    std::vector<uint16_t> slots;
    Equipamiento equipamiento;

    size_t cantidadSlotsLibres() const;
};

#endif
