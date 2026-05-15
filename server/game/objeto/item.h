#ifndef ITEM_H
#define ITEM_H

#include <cstdint>

enum class TipoItem {
    Arma,
    Baculo,
    Defensa,
    Pocion
};
class Item {
    uint16_t idItem;
    std::string nombre;
    TipoItem tipo;
};

class Arma : public Item {
    uint8_t danioMin;
    uint8_t danioMax;
    bool esDistancia;
};


enum class TipoHechizo {
    FlechaMagica,
    Curar,
    Misil,
    Explosion
};
class Baculo : public Item {
    uint8_t danioMin;
    uint8_t danioMax;
    uint16_t hechizo;
    uint16_t costoMana;
};


enum class TipoDefensa {
    Escudo,
    Armadura,
    Casco
};
class Defensa : public Item {
    uint8_t defMin;
    uint8_t defMax;
    TipoDefensa slot;
};


enum class TipoPocion {
    Vida,
    Mana
};
class Pocion : public Item {
    uint16_t cantidad;
    TipoPocion tipo;
};

#endif