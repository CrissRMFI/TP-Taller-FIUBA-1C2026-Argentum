#ifndef JUGADOR_H
#define JUGADOR_H

#include <cstdint>
#include <string>
#include <vector>

#include "objeto/inventario.h"
#include "modelo/posicion.h"
#include "modelo/raza.h"
#include "modelo/clase_personaje.h"

enum class Estado {
    Vivo,
    Fantasma,
    Meditando,
    Resucitando
};

class Jugador {
public:
    Jugador(const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion);

    // Modificadores de vida y maná
    void recibir_danio(uint16_t cantidad);
    void curar(uint16_t cantidad);
    void recuperar_mana(uint16_t cantidad);

    // Experiencia y progresión
    void ganar_experiencia(uint32_t cantidad);
    void sumar_oro(uint32_t cantidad);
    bool gastar_oro(uint32_t cantidad);

    // Movimiento y estado
    void mover_a(uint16_t x, uint16_t y);
    void resucitar(uint16_t x, uint16_t y);
    void meditar();
    uint16_t calcular_danio();

    // Inventario
    bool agregar_item(uint16_t idItem);
    bool eliminar_item(uint16_t idItem);
    bool equipar_item(uint16_t idItem);
    void agregar_item_banco(uint16_t idItem);
    void agregar_oro_banco(uint32_t cantidad);
    bool sacar_item_banco(uint16_t idItem);
    bool sacar_oro_banco(uint32_t cantidad);

    // Getters
    uint16_t getId() const;
    bool es_newbie() const;
    uint8_t getNivel() const;
    bool fundo_clan() const;
    std::string getNombre() const;
    Posicion getPosicion() const;
    Estado getEstado() const;
    std::vector<uint16_t> getIdItemsBanco() const;
    uint32_t getOroBanco() const;
    uint16_t getClan() const;

private:
    uint16_t idJugador;
    uint16_t idClan;
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
    std::vector<uint16_t> idItemsBanco;
    ClasePersonaje clase;
    Estado estado;
    Raza raza;
    bool fundadoClan;

    void subirNivel();
    void morir();
    void perder_experiencia(uint32_t cantidad);
    bool consumir_mana(uint16_t cantidad);
    void consumir_item(uint16_t idItem);
    bool esquiva_ataque();
    bool es_golpe_critico();
};

#endif
