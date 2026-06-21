#include "catalogo_items.h"

void CatalogoItems::registrar(uint16_t id, std::unique_ptr<Item> item) {
    items[id] = std::move(item);
}

const Item* CatalogoItems::buscar(uint16_t id) const {
    auto it = items.find(id);
    return it != items.end() ? it->second.get() : nullptr;
}

bool CatalogoItems::existe(uint16_t id) const {
    return items.count(id) > 0;
}

std::vector<uint16_t> CatalogoItems::idsDisponibles() const {
    std::vector<uint16_t> ids;
    ids.reserve(items.size());

    for (const auto& [id, item] : items) {
        ids.push_back(id);
    }

    return ids;
}

const Arma* CatalogoItems::comoArma(uint16_t id) const {
    const Item* item = buscar(id);
    if (!item || item->getTipo() != TipoItem::Arma)
        return nullptr;
    return static_cast<const Arma*>(item);
}

const Baculo* CatalogoItems::comoBaculo(uint16_t id) const {
    const Item* item = buscar(id);
    if (!item || item->getTipo() != TipoItem::Baculo)
        return nullptr;
    return static_cast<const Baculo*>(item);
}

const Defensa* CatalogoItems::comoDefensa(uint16_t id) const {
    const Item* item = buscar(id);
    if (!item || item->getTipo() != TipoItem::Defensa)
        return nullptr;
    return static_cast<const Defensa*>(item);
}

const Pocion* CatalogoItems::comoPocion(uint16_t id) const {
    const Item* item = buscar(id);
    if (!item || item->getTipo() != TipoItem::Pocion)
        return nullptr;
    return static_cast<const Pocion*>(item);
}
