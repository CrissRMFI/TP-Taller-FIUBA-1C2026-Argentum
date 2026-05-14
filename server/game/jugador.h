#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <cstdint>

#include "inventario.h"
#include "../gameloop/mensaje_salida.h"
#include "items.h"

// Pongo aca para que sea mas facil de manejar, luego hay que moverlos
enum class Estado {
    Vivo,
    Fantasma,
    Meditando,
    Resucitando
};

enum class Clase {
    Guerrero,
    Mago,
    Clerigo,
    Paladin
};

enum class Raza {
    Humano,
    Elfo,
    Enano,
    Gnomo
};

struct Posicion {
    uint16_t x;
    uint16_t y;
    uint16_t mapaId;
};

class Jugador {
public:
    Jugador(const std::string& nombre, Clase clase, Raza raza, Posicion posicion);
    // Modificadores de Vida
    void recibir_danio(uint16_t cantidad);
    void curar(uint16_t cantidad);

    // Modificadores de Maná
    void recuperar_mana(uint16_t cantidad);

    // Experiencia y Progresión
    void ganar_experiencia(uint32_t cantidad);
    void sumar_oro(uint32_t cantidad);
    bool gastar_oro(uint32_t cantidad);

    // Movimiento y Estado
    void mover_a(uint16_t x, uint16_t y);
    void resucitar(uint16_t x, uint16_t y);
    void meditar();
    uint16_t calcular_danio();

    //Objetos e Inventario
    bool agregar_item(const uint16_t idItem);
    bool eliminar_item(const uint16_t idItem);
    bool equipar_item(const uint16_t idItem);
    void agregar_item_banco(uint16_t idItem);
    void agregar_oro_banco(uint32_t cantidad);
    bool sacar_item_banco(uint16_t idItem);
    bool sacar_oro_banco(uint32_t cantidad);

    // Getters
    bool es_newbie() const; // Para el fair play
    uint8_t getNivel() const; // Para el fair play
    bool fundo_clan() const;
    std::string getNombre() const;
    Posicion getPosicion() const;
    Estado getEstado() const;
    std::vector<Item> getItemsBanco() const;
    uint32_t getOroBanco() const;

private:
    uint16_t idJugador;
    std::string nombre;
    uint8_t nivel;
    uint32_t experiencia;
    uint16_t vidaActual;
    uint16_t vidaMax;
    uint16_t manaActual;
    uint16_t manaMax;
    uint32_t oroMano;
    uint32_t oroExceso;
    uint32_t oroBanco;
    bool esFantasma;
    bool estaMeditando;
    uint8_t fuerza;
    uint8_t agilidad;
    uint8_t inteligencia;
    uint8_t constitucion;
    Posicion posicion;
    Inventario inventario;
    std::vector<Item> itemsBanco;
    Clase clase;
    Estado estado;
    Raza raza;
    bool fundo_clan;

    void subirNivel();
    void morir(); // Deberia devolver lo que dropea
    void perder_experiencia(uint32_t cantidad);
    bool consumir_mana(uint16_t cantidad);
    void consumir_item(const uint16_t idItem); // Cuando se equipa un item consumible
    bool esquiva_ataque();
    bool es_golpe_critico();
};

#endif