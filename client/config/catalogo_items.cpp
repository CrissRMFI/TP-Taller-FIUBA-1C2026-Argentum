#include "catalogo_items.h"

#include <cctype>
#include <iostream>

#include <toml++/toml.hpp>

#include "../../common/mensajes/mensajes_error_cliente.h"

namespace {

std::string aNombreLindo(const std::string& clave) {
    std::string n = clave;
    for (char& c : n) {
        if (c == '_') {
            c = ' ';
        }
    }
    if (!n.empty()) {
        n[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(n[0])));
    }
    return n;
}

}

CatalogoItems::CatalogoItems(const std::string& gameConfigPath) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(gameConfigPath);
    } catch (const toml::parse_error& e) {
        std::cerr << "[cliente] "
                  << MensajesErrorCliente::mensaje(CodigoErrorCliente::ITEMS_NO_LEIDOS) << " ('"
                  << gameConfigPath << "': " << e.description() << ")" << std::endl;
        return;
    }

    const toml::table* items = tbl["items"].as_table();
    if (items == nullptr) {
        return;
    }

    
    for (const auto& [clave, valor] : *items) {
        const toml::table* item = valor.as_table();
        if (item == nullptr) {
            continue;
        }
        const std::optional<int64_t> id = (*item)["id"].value<int64_t>();
        if (!id || *id < 0 || *id > 0xFFFF) {
            continue;
        }
        ItemInfo info;
        info.id = static_cast<uint16_t>(*id);
        info.clave = std::string(clave.str());
        info.nombre = aNombreLindo(info.clave);
        info.tipo = (*item)["tipo"].value_or<std::string>("");

        porId[info.id] = info;
        claveAId[info.clave] = info.id;
    }
    
    if (const toml::table* hechizosTbl = tbl["hechizos"].as_table()) {
        for (const auto& [clave, valor] : *hechizosTbl) {
            const toml::table* h = valor.as_table();
            if (h == nullptr) {
                continue;
            }
            const std::optional<int64_t> id = (*h)["id"].value<int64_t>();
            if (!id || *id < 0 || *id > 0xFFFF) {
                continue;
            }
            HechizoInfo hi;
            hi.id = static_cast<uint16_t>(*id);
            hi.nombre = (*h)["nombre"].value_or<std::string>(aNombreLindo(std::string(clave.str())));
            hi.tipo = (*h)["tipo"].value_or<std::string>("danio");
            hi.mana = static_cast<uint16_t>((*h)["mana"].value_or<int64_t>(0));
            hi.precio = static_cast<uint32_t>((*h)["precio"].value_or<int64_t>(0));
            hechizos[hi.id] = hi;
        }
    }

    if (const toml::array* stock = tbl["npcs"]["comerciante"]["items"].as_array()) {
        for (const toml::node& nodo : *stock) {
            const toml::table* entrada = nodo.as_table();
            if (entrada == nullptr) {
                continue;
            }
            const std::optional<int64_t> id = (*entrada)["id"].value<int64_t>();
            if (!id || *id < 0 || *id > 0xFFFF) {
                continue;
            }
            const auto iid = static_cast<uint16_t>(*id);
            compraComerciante[iid] =
                    static_cast<uint32_t>((*entrada)["compra"].value_or<int64_t>(0));
            ventaComerciante[iid] =
                    static_cast<uint32_t>((*entrada)["venta"].value_or<int64_t>(0));
        }
    }
}

const ItemInfo* CatalogoItems::info(uint16_t id) const {
    const auto it = porId.find(id);
    return (it != porId.end()) ? &it->second : nullptr;
}

std::string CatalogoItems::nombre(uint16_t id) const {
    const auto it = porId.find(id);
    return (it != porId.end()) ? it->second.nombre : ("Item " + std::to_string(id));
}

const std::unordered_map<std::string, uint16_t>& CatalogoItems::mapaClaveAId() const {
    return claveAId;
}

uint32_t CatalogoItems::precioCompra(uint16_t id) const {
    const auto it = compraComerciante.find(id);
    return (it != compraComerciante.end()) ? it->second : 0;
}

uint32_t CatalogoItems::precioVenta(uint16_t id) const {
    const auto it = ventaComerciante.find(id);
    return (it != ventaComerciante.end()) ? it->second : 0;
}

const HechizoInfo* CatalogoItems::hechizo(uint16_t id) const {
    const auto it = hechizos.find(id);
    return (it != hechizos.end()) ? &it->second : nullptr;
}

std::vector<uint16_t> CatalogoItems::idsHechizos() const {
    std::vector<uint16_t> ids;
    ids.reserve(hechizos.size());
    for (const auto& [id, h] : hechizos) {
        ids.push_back(id);
    }
    return ids;
}
