#ifndef ITEM_H
#define ITEM_H

#include <cstdint>
#include <string>

enum class TipoItem { Arma, Baculo, Defensa, Pocion };

enum class TipoHechizo { FlechaMagica, Curar, Misil, Explosion };

enum class TipoDefensa { Escudo, Armadura, Casco };

enum class TipoPocion { Vida, Mana };

class Item {
public:
    Item(uint16_t id, const std::string& nombre, TipoItem tipo) :
            idItem(id), nombre(nombre), tipo(tipo) {}
    virtual ~Item() = default;

    uint16_t getId() const {
        return idItem;
    }
    TipoItem getTipo() const {
        return tipo;
    }
    const std::string& getNombre() const {
        return nombre;
    }
    // Sprite overlay que el item muestra al equiparse (arma/escudo/casco). 0 = ninguno.
    void setSpriteEquip(uint16_t sprite) {
        spriteEquip = sprite;
    }
    uint16_t getSpriteEquip() const {
        return spriteEquip;
    }
    // Sprite de la animacion de ataque (swing) de esta arma. 0 = ninguno.
    void setSpriteAtaque(uint16_t sprite) {
        spriteAtaque = sprite;
    }
    uint16_t getSpriteAtaque() const {
        return spriteAtaque;
    }

private:
    uint16_t idItem;
    std::string nombre;
    TipoItem tipo;
    uint16_t spriteEquip = 0;
    uint16_t spriteAtaque = 0;
};

class Arma : public Item {
public:
    Arma(uint16_t id, const std::string& nombre, uint8_t danioMin, uint8_t danioMax,
         bool esDistancia) :
            Item(id, nombre, TipoItem::Arma),
            danioMin(danioMin),
            danioMax(danioMax),
            esDistancia(esDistancia) {}

    uint8_t getDanioMin() const {
        return danioMin;
    }
    uint8_t getDanioMax() const {
        return danioMax;
    }
    bool esArmaDistancia() const {
        return esDistancia;
    }

private:
    uint8_t danioMin;
    uint8_t danioMax;
    bool esDistancia;
};

class Baculo : public Item {
public:
    Baculo(uint16_t id, const std::string& nombre, uint8_t danioMin, uint8_t danioMax,
           TipoHechizo hechizo, uint16_t costoMana) :
            Item(id, nombre, TipoItem::Baculo),
            danioMin(danioMin),
            danioMax(danioMax),
            hechizo(hechizo),
            costoMana(costoMana) {}

    uint8_t getDanioMin() const {
        return danioMin;
    }
    uint8_t getDanioMax() const {
        return danioMax;
    }
    TipoHechizo getHechizo() const {
        return hechizo;
    }
    uint16_t getCostoMana() const {
        return costoMana;
    }

private:
    uint8_t danioMin;
    uint8_t danioMax;
    TipoHechizo hechizo;
    uint16_t costoMana;
};

class Defensa : public Item {
public:
    Defensa(uint16_t id, const std::string& nombre, uint8_t defMin, uint8_t defMax,
            TipoDefensa slot, uint16_t spriteCuerpo = 0) :
            Item(id, nombre, TipoItem::Defensa),
            defMin(defMin),
            defMax(defMax),
            slot(slot),
            spriteCuerpo(spriteCuerpo) {}

    uint8_t getDefMin() const {
        return defMin;
    }
    uint8_t getDefMax() const {
        return defMax;
    }
    TipoDefensa getSlot() const {
        return slot;
    }
    uint16_t getSpriteCuerpo() const {
        return spriteCuerpo;
    }

private:
    uint8_t defMin;
    uint8_t defMax;
    TipoDefensa slot;
    uint16_t spriteCuerpo;
};

class Pocion : public Item {
public:
    Pocion(uint16_t id, const std::string& nombre, uint16_t cantidad, TipoPocion tipoPocion) :
            Item(id, nombre, TipoItem::Pocion), cantidad(cantidad), tipoPocion(tipoPocion) {}

    uint16_t getCantidad() const {
        return cantidad;
    }
    TipoPocion getTipoPocion() const {
        return tipoPocion;
    }

private:
    uint16_t cantidad;
    TipoPocion tipoPocion;
};

#endif
