#ifndef JUGADOR_H
#define JUGADOR_H

#include <cstdint>
#include <string>
#include <vector>

#include "config/config_juego.h"
#include "modelo/clase_personaje.h"
#include "modelo/posicion.h"
#include "modelo/raza.h"
#include "objeto/inventario.h"

class CatalogoItems;

enum class Estado {
    Vivo,
    Fantasma,
    Meditando,
};

class Jugador {
public:
    Jugador(uint16_t id,
            const std::string& nombre,
            ClasePersonaje clase,
            Raza raza,
            Posicion posicion,
            const ConfigJuego& cfg);

    // Modificadores de vida y maná
    void recibir_danio(uint16_t cantidad);
    uint16_t recibir_ataque_fisico(uint16_t danio, const CatalogoItems& catalogo);
    void curar(uint16_t cantidad);
    void recuperar_mana(uint16_t cantidad);

    // Recuperación periódica
    void recuperar(float segundos);

    // Experiencia, oro y progresión
    void ganar_experiencia(uint32_t cantidad);
    void sumar_oro(uint32_t cantidad);
    bool gastar_oro(uint32_t cantidad);

    // Movimiento y estado
    void mover_a(uint16_t x, uint16_t y);
    void resucitar(uint16_t x, uint16_t y);
    void inmovilizar(uint16_t resucitarX, uint16_t resucitarY, float segundos);
    void meditar();
    void cancelarMeditacion();
    uint16_t calcular_danio(const CatalogoItems& catalogo);

    // Inventario
    bool agregar_item(uint16_t idItem);
    bool eliminar_item(uint16_t idItem);
    bool equipar_item(uint8_t indice, const CatalogoItems& catalogo);
    bool agregar_item_banco(uint8_t indice);
    bool agregar_oro_banco(uint32_t cantidad);
    bool sacar_item_banco(uint16_t idItem);
    bool sacar_oro_banco(uint32_t cantidad);
    uint16_t quitar_item_de_slot(uint8_t indice);
    std::vector<uint16_t> vaciar_inventario();

    // Clan
    void asignarClan(uint16_t idClan);
    void salirClan();
    void marcarFundadorClan();

    // Queries de estado
    bool puedeMeditar() const;
    bool puedeUsarMagia() const;
    bool estaVivo() const;
    bool esFantasma() const;
    bool enMeditacion() const;
    bool tieneClan() const;

    // Getters
    uint16_t getId() const;
    uint8_t getNivel() const;
    uint32_t getExperiencia() const;
    uint16_t getVidaActual() const;
    uint16_t getVidaMax() const;
    uint16_t getManaActual() const;
    uint16_t getManaMax() const;
    uint32_t getOro() const;
    uint32_t getOroBanco() const;
    uint16_t getClan() const;
    bool estaInmovilizado() const;
    bool fundo_clan() const;
    bool es_newbie() const;
    std::string getNombre() const;
    Posicion getPosicion() const;
    Posicion getPosicionResurreccion() const;
    Estado getEstado() const;
    std::vector<uint16_t> getSlotsInventario() const;
    uint16_t getArmaEquipada() const;
    uint16_t getBaculoEquipado() const;
    uint16_t getDefensaEquipada() const;
    uint16_t getCascoEquipado() const;
    uint16_t getEscudoEquipado() const;
    std::vector<uint16_t> getIdItemsBanco() const;

    void actualizarId(uint16_t nuevoId);

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
    float recuperacionVidaPendiente;
    float recuperacionManaPendiente;
    float meditacionManaPendiente;

    uint32_t oroMano;
    uint32_t oroExceso;
    uint32_t oroBanco;

    uint8_t fuerza;
    uint8_t agilidad;
    uint8_t inteligencia;
    uint8_t constitucion;

    Posicion posicion;
    Posicion posicionResurreccion;
    ConfigJuego cfg;
    Inventario inventario;

    std::vector<uint16_t> idItemsBanco;

    ClasePersonaje clase;
    Estado estado;
    Raza raza;
    bool fundadoClan;
    float tiempoRestanteInmovilizado;

    void subirNivel();
    void morir();
    void perder_experiencia(uint32_t cantidad);
    bool consumir_mana(uint16_t cantidad);
    void consumir_item(uint16_t idItem);
    void normalizarOro();
    bool esquiva_ataque();
    bool es_golpe_critico();
};

#endif
