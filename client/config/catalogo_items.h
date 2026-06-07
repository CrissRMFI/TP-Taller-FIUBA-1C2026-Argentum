#ifndef CLIENT_CONFIG_CATALOGO_ITEMS_H
#define CLIENT_CONFIG_CATALOGO_ITEMS_H

#include <cstdint>
#include <string>
#include <unordered_map>

// Datos de un item que el cliente necesita para el panel/inventario.
struct ItemInfo {
    uint16_t id = 0;
    std::string nombre;
    std::string clave;
    std::string tipo;
};


class CatalogoItems {
private:
    std::unordered_map<uint16_t, ItemInfo>     porId;
    std::unordered_map<std::string, uint16_t>  claveAId;
public:
    explicit CatalogoItems(const std::string& gameConfigPath);

    const ItemInfo* info(uint16_t id) const;
    std::string nombre(uint16_t id) const;
    const std::unordered_map<std::string, uint16_t>& mapaClaveAId() const;

    uint32_t precioCompra(uint16_t id) const;
    uint32_t precioVenta(uint16_t id) const;

private:
    std::unordered_map<uint16_t, uint32_t> compraComerciante;
    std::unordered_map<uint16_t, uint32_t> ventaComerciante;
};

#endif
