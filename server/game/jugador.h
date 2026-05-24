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

class Aleatorio;
class CatalogoItems;

enum class Estado {
    Vivo,
    Fantasma,
    Meditando,
    Resucitando,
};

// Resultado del cálculo de daño físico de un atacante. El flag `esCritico`
// viaja explícito para que el defensor pueda aplicar la regla 5.2 (el crítico
// omite la fase de evasión). Sin este flag la información se perdía dentro de
// `calcular_danio`.
struct ResultadoDanio {
    uint16_t valor;
    bool esCritico;
};

// Resultado de aplicar un ataque físico sobre el defensor. Discrimina esquive
// vs golpe efectivo para que el caller pueda emitir EventoEsquive con la
// misma semántica que ya usa el flujo PvE (juego.cpp:2010-2019). Sin esto,
// el cliente recibe `cantidad == 0` tanto en esquive como en absorción total
// y no puede diferenciar la animación correcta.
struct ResultadoDefensa {
    enum class Tipo : uint8_t { Golpeado, Esquivo };
    Tipo tipo;
    uint16_t danioAplicado;  // 0 si tipo == Esquivo
};

// Tipo de ataque que el jugador ejecuta según su equipamiento.
// `Juego` lo consulta vía `Jugador::describir_ataque` para decidir si valida
// adyacencia (melee) o rango de visión (distancia/hechizo).
enum class TipoAtaque {
    CuerpoACuerpo,   // sin arma o arma melee: alcance == 1
    Distancia,       // Arma::esArmaDistancia() == true
    Hechizo,         // Baculo de daño equipado (consume maná)
    HechizoNoOfensivo // Curar o magia no permitida para ATACAR
};

// Descriptor del próximo ataque del jugador. Lo emite `describir_ataque`
// para que `Juego` valide alcance y costo de maná antes de ejecutar.
struct DescriptorAtaque {
    TipoAtaque tipo;
    uint16_t alcanceMaximo;  // distancia Manhattan máxima válida hasta el objetivo
    uint16_t costoMana;      // maná que el atacante debe pagar al ejecutar
};

class Jugador{
public:
    Jugador(uint16_t id,
            const std::string& nombre,
            ClasePersonaje clase,
            Raza raza,
            Posicion posicion,
            const ConfigJuego& cfg);

    // Modificadores de vida y maná
    void recibir_danio(uint16_t cantidad);

    // Aplica daño físico al jugador. Si `esCritico` es true se saltea la fase
    // de evasión (regla 5.2). La absorción por armadura/casco/escudo (regla
    // 5.5) se aplica cuando no hay esquive. Devuelve un ResultadoDefensa que
    // distingue esquive de golpe (con su daño final entrado a vida).
    // `aleatorio` se inyecta para muestrear las tiradas de evasión y absorción.
    ResultadoDefensa recibir_ataque_fisico(uint16_t danio,
                                           bool esCritico,
                                           const CatalogoItems& catalogo,
                                           Aleatorio& aleatorio,
                                           float multiplicadorDefensa = 1.0f);

    void curar(uint16_t cantidad);
    void recuperar_mana(uint16_t cantidad);

    // Recuperación periódica
    void recuperar(float segundos);

    // Experiencia, oro y progresión
    void ganar_experiencia(uint32_t cantidad);
    void sumar_oro(uint32_t cantidad);
    bool gastar_oro(uint32_t cantidad);
    uint32_t extraer_oro_perdido();
    bool puede_recibir_oro(uint32_t cantidad) const;

    // Movimiento y estado
    void mover_a(uint16_t x, uint16_t y);
    void resucitar(uint16_t x, uint16_t y);
    void inmovilizar(uint16_t resucitarX, uint16_t resucitarY, float segundos);
    void meditar();
    void cancelarMeditacion();
    // Calcula el daño del próximo golpe del jugador y reporta si fue crítico.
    // El consumidor necesita el flag para invocar `recibir_ataque_fisico` con
    // la semántica correcta de la regla 5.2. `aleatorio` se inyecta para
    // muestrear el daño base del arma y la tirada de crítico.
    ResultadoDanio calcular_danio(const CatalogoItems& catalogo, Aleatorio& aleatorio);

    // Describe el próximo ataque según el equipamiento actual. `Juego` lo
    // consulta para validar alcance (regla 5.3) y maná (regla 3.2.1) sin
    // tener que inspeccionar Arma/Baculo del catálogo por su cuenta.
    DescriptorAtaque describir_ataque(const CatalogoItems& catalogo) const;

    // Consume `cantidad` puntos de maná si hay suficiente; devuelve true en ese
    // caso. Útil para que `Juego` cobre el costo de un hechizo de báculo antes
    // de calcular daño.
    bool consumir_mana(uint16_t cantidad);

    // Inventario
    bool puede_agregar_item(uint16_t idItem) const;
    bool agregar_item(uint16_t idItem);
    bool eliminar_item(uint16_t idItem);
    bool equipar_item(uint8_t indice, const CatalogoItems& catalogo);
    bool agregar_item_banco(uint8_t indice);
    bool agregar_oro_banco(uint32_t cantidad);
    bool sacar_item_banco(uint16_t idItem);
    bool sacar_oro_banco(uint32_t cantidad);
    uint16_t quitar_item_de_slot(uint8_t indice);
    void agregar_item_en_slot(uint16_t idItem, uint8_t indice);
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

    // Peek no destructivo de un slot del inventario. Devuelve el idItem que
    // hay en `indice`, o 0 si el slot está vacío / el índice es inválido.
    // Permite validar antes de quitarlo.
    uint16_t getIdItemEnSlot(uint8_t indice) const;
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
    uint32_t oroPerdidoPendiente;
    uint32_t oroBanco;

    uint8_t fuerza;
    uint8_t agilidad;
    uint8_t inteligencia;
    uint8_t constitucion;

    Posicion posicion;
    Posicion posicionResurreccion;
    const ConfigJuego& cfg;
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
    void consumir_item(uint16_t idItem);
    void normalizarOro();
    bool esquiva_ataque(Aleatorio& aleatorio);
    bool es_golpe_critico(Aleatorio& aleatorio);
};

#endif
