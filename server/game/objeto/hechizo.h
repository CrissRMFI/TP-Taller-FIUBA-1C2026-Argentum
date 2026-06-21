#ifndef HECHIZO_H
#define HECHIZO_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// Efecto del hechizo sobre el objetivo: baja HP (danio) o sube HP (cura).
enum class TipoHechizoEfecto { Danio, Cura };

struct Hechizo {
    uint16_t id;
    std::string nombre;
    TipoHechizoEfecto tipo;
    uint16_t min;     // puntos minimos de danio/cura
    uint16_t max;     // puntos maximos de danio/cura
    uint16_t mana;    // costo de mana al lanzar
    uint32_t precio;  // costo en oro al comprarlo al sacerdote
};

class CatalogoHechizos {
public:
    void registrar(const Hechizo& h) {
        hechizos[h.id] = h;
    }

    const Hechizo* buscar(uint16_t id) const {
        const auto it = hechizos.find(id);
        return it != hechizos.end() ? &it->second : nullptr;
    }

    bool existe(uint16_t id) const {
        return hechizos.find(id) != hechizos.end();
    }

    std::vector<uint16_t> idsDisponibles() const {
        std::vector<uint16_t> ids;
        ids.reserve(hechizos.size());
        for (const auto& [id, h] : hechizos) {
            ids.push_back(id);
        }
        return ids;
    }

private:
    std::map<uint16_t, Hechizo> hechizos;
};

#endif
