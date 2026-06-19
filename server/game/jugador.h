#ifndef JUGADOR_H
#define JUGADOR_H

#include <cstdint>
#include <string>
#include <vector>

#include "config/config_juego.h"
#include "modelo/clase_personaje.h"
#include "../../common/game/modelo/posicion.h"
#include "modelo/raza.h"
#include "objeto/inventario.h"

class Aleatorio;
class CatalogoItems;
class SerializadorJugador;

enum class Estado {
    Vivo,
    Fantasma,
    Meditando,
    Resucitando,
};

// Resultado del cálculo de daño físico de un atacante. El flag `esCritico` viaja explícito para que el defensor pueda aplicar que el crítico omite la fase de evasión. Sin este flag la información se pierde al calcular el danio
struct ResultadoDanio {
    uint16_t valor;
    bool esCritico;
};

// Resultado de aplicar un ataque físico sobre el defensor. Discrimina esquive vs golpe efectivo para que el caller pueda emitir EventoEsquive con la misma semántica que ya usa el flujo Sin esto, el cliente recibe `cantidad == 0` tanto en esquive como en absorción total y no puede diferenciar .
struct ResultadoDefensa {
    enum class Tipo : uint8_t { Golpeado, Esquivo };
    Tipo tipo;
    uint16_t danioAplicado;  // 0 si tipo == Esquivo
};

// Tipo de ataque que el jugador ejecuta según su equipamiento.
// Juego lo consulta vía Jugador::describir_ataque para decidir si valida adyacencia o rango de visión (distancia/hechizo).
enum class TipoAtaque {
    CuerpoACuerpo,   // sin arma: alcance == 1
    Distancia,       // Arma::esArmaDistancia() == true
    Hechizo,         // Baculo de daño equipado (consume maná)
    HechizoNoOfensivo // Curar o magia no permitida para ATACAR
};

// Descriptor del próximo ataque del jugador. Lo emite `describir_ataque` para que `Juego` valide alcance y costo de maná antes de ejecutar.
struct DescriptorAtaque {
    TipoAtaque tipo;
    uint16_t alcanceMaximo;  // distancia Manhattan máxima válida hasta el objetivo
    uint16_t costoMana;      // maná que el atacante debe pagar al ejecutar
};

struct DatosRestauracion {
    uint16_t idClan;
    uint16_t skinCabeza;
    uint16_t skinCuerpo;
    bool     fundadoClan;
    Estado   estado;
    uint8_t  nivel;
    uint32_t experiencia;
    uint16_t vidaActual;
    uint16_t manaActual;
    uint32_t oroMano;
    uint32_t oroExceso;
    uint32_t oroBanco;
    uint32_t oroPerdidoPendiente;
    std::vector<uint16_t> slotsInventario;
    uint16_t equipArma;
    uint16_t equipBaculo;
    uint16_t equipDefensa;
    uint16_t equipCasco;
    uint16_t equipEscudo;
    std::vector<uint16_t> itemsBanco;
};

class Jugador{
public:
    Jugador(uint16_t id,
            const std::string& nombre,
            ClasePersonaje clase,
            Raza raza,
            uint16_t cabeza,
            uint16_t cuerpo,
            Posicion posicion,
            const ConfigJuego& cfg);

    void restaurar(const DatosRestauracion& datos);
    void recalcularVestimenta(const CatalogoItems& catalogo);

    // Modificadores de vida y maná
    void recibir_danio(uint16_t cantidad);

    // Aplica daño físico al jugador. Si `esCritico` es true se saltea la fase de evasión. La absorción por armadura/casco/escudo se aplica cuando no hay esquive. Devuelve un ResultadoDefensa que distingue esquive de golpe (con su daño final entrado a vida) `aleatorio` se inyecta para muestrear las tiradas de evasión y absorción.
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
    void cheat_dar_oro(uint32_t cantidad);
    bool gastar_oro(uint32_t cantidad);
    uint32_t extraer_oro_perdido();
    bool puede_recibir_oro(uint32_t cantidad) const;

    // Movimiento y estado
    void mover_a(uint16_t x, uint16_t y);
    // Teletransporte: cambia la posicion completa (mapaId + x + y), p. ej. al cruzar un portal hacia otra mazmorra.
    void reubicar(const Posicion& nuevaPosicion);
    // Movimiento continuo server-driven: el cliente avisa empezar/detener y el
    // servidor avanza una celda cada N ticks mientras el jugador siga moviendose.
    void empezarMover(uint8_t direccion);
    void detenerMover();
    bool estaMoviendose() const;
    uint8_t getDireccionMov() const;
    bool debeAvanzar(uint16_t ticksPorCelda);
    void resucitar(uint16_t x, uint16_t y);
    void inmovilizar(uint16_t resucitarX, uint16_t resucitarY, float segundos);
    void meditar();
    void cancelarMeditacion();
    // Calcula el daño del próximo golpe del jugador y reporta si fue crítico. 
    ResultadoDanio calcular_danio(const CatalogoItems& catalogo, Aleatorio& aleatorio);

    // Describe el próximo ataque según el equipamiento actual. `Juego` lo consulta para validar alcance y maná
    DescriptorAtaque describir_ataque(const CatalogoItems& catalogo) const;

    // Consume `cantidad` puntos de maná si hay suficiente; devuelve true en ese caso. Útil para que `Juego` cobre el costo de un hechizo de báculo antes de calcular daño.
    bool consumir_mana(uint16_t cantidad);

    // Cooldown de ataque: `puedeAtacar` indica si ya transcurrió el tiempo de enfriamiento (cfg.cooldownAtaqueSeg) desde el último golpe. `registrarAtaque`
    // reinicia ese contador cuando el jugador efectivamente ejecuta un ataque.
    bool puedeAtacar() const;
    void registrarAtaque();

    // Inventario
    bool puede_agregar_item(uint16_t idItem) const;
    bool agregar_item(uint16_t idItem);
    bool eliminar_item(uint16_t idItem);
    bool equipar_item(uint8_t indice, const CatalogoItems& catalogo);
    bool desequipar_item(uint8_t ranura, const CatalogoItems& catalogo);
    bool agregar_item_banco(uint8_t indice);
    bool agregar_oro_banco(uint32_t cantidad);
    bool sacar_item_banco(uint16_t idItem);
    bool sacar_oro_banco(uint32_t cantidad);
    uint16_t quitar_item_de_slot(uint8_t indice);
    void agregar_item_en_slot(uint16_t idItem, uint8_t indice);
    std::vector<uint16_t> vaciar_inventario();

    // Hechizos: se compran al sacerdote y se lanzan sobre un objetivo.
    bool conoceHechizo(uint16_t idHechizo) const;
    void aprenderHechizo(uint16_t idHechizo);
    const std::vector<uint16_t>& getHechizosConocidos() const;

    // Cheats de prueba. vida/mana infinitos son toggles; matar fuerza la muerte
    // al instante ignorando la invulnerabilidad de cheat.
    void alternarVidaInfinita();
    void alternarManaInfinito();
    void matar();

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
    uint32_t getOroMano() const;
    uint32_t getOroExceso() const;
    uint32_t getOroBanco() const;
    uint32_t getOroPerdidoPendiente() const;
    uint16_t getClan() const;
    uint16_t getCabeza() const;
    uint16_t getCuerpo() const;
    uint16_t getSpriteArma() const;
    uint16_t getSpriteEscudo() const;
    uint16_t getSpriteCasco() const;
    ClasePersonaje getClase() const;
    Raza getRaza() const;
    bool estaInmovilizado() const;
    // Segundos que aun le restan inmovil mientras resucita (0 si no esta resucitando).
    float getTiempoRestanteInmovilizado() const;
    bool fundo_clan() const;
    bool es_newbie() const;
    std::string getNombre() const;
    Posicion getPosicion() const;
    Posicion getPosicionResurreccion() const;
    Estado getEstado() const;
    std::vector<uint16_t> getSlotsInventario() const;

    // Peek no destructivo de un slot del inventario. Devuelve el idItem que hay en indice, o 0 si el slot está vacío / el índice es inválido. Permite validar antes de quitarlo.
    uint16_t getIdItemEnSlot(uint8_t indice) const;
    uint16_t getArmaEquipada() const;
    uint16_t getBaculoEquipado() const;
    uint16_t getDefensaEquipada() const;
    uint16_t getCascoEquipado() const;
    uint16_t getEscudoEquipado() const;
    std::vector<uint16_t> getIdItemsBanco() const;

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
    std::vector<uint16_t> hechizosConocidos;

    ClasePersonaje clase;
    Estado estado;
    Raza raza;
    uint16_t cabeza;
    uint16_t cuerpo;
    uint16_t cuerpoBase;
    uint16_t spriteArma = 0; // overlay del arma/baculo equipado (vestimenta)
    uint16_t spriteEscudo = 0; // overlay del escudo equipado
    uint16_t spriteCasco = 0; // overlay del casco equipado
    bool fundadoClan;
    float tiempoRestanteInmovilizado;
    float tiempoDesdeUltimoAtaque;

    // Flags de cheat (transitorios, no se persisten).
    bool vidaInfinita;
    bool manaInfinito;

    // Estado de movimiento continuo (transitorio, no se persiste).
    bool moviendose;
    uint8_t direccionMov;
    uint16_t ticksAcumuladosMov;

    void subirNivel();
    void morir();
    void perder_experiencia(uint32_t cantidad);
    void consumir_item(uint16_t idItem);
    // Recalcula la vestimenta a renderizar (cuerpo + overlays arma/escudo/casco) segun el equipamiento actual. Se llama tras cada equipar/desequipar.
    void actualizarVestimenta(const CatalogoItems& catalogo);
    void normalizarOro();
    bool esquiva_ataque(Aleatorio& aleatorio);
    bool es_golpe_critico(Aleatorio& aleatorio);
};

#endif
