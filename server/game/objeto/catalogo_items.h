#ifndef CATALOGO_ITEMS_H
#define CATALOGO_ITEMS_H

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include "item.h"

class CatalogoItems {
public:
    void registrar(uint16_t id, std::unique_ptr<Item> item);

    const Item*    buscar   (uint16_t id) const;
    const Arma*    comoArma  (uint16_t id) const;
    const Baculo*  comoBaculo(uint16_t id) const;
    const Defensa* comoDefensa(uint16_t id) const;
    const Pocion*  comoPocion (uint16_t id) const;

    bool existe(uint16_t id) const;
    std::vector<uint16_t> idsDisponibles() const;

private:
    std::map<uint16_t, std::unique_ptr<Item>> items;
};

#endif
