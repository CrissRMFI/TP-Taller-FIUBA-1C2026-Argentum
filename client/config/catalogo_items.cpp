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
