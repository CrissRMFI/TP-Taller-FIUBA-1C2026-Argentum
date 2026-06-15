#ifndef EDITOR_MAPA_H
#define EDITOR_MAPA_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "common/game/criatura.h"
#include "common/game/mapa/mapa.h"
#include "common/game/modelo/posicion.h"
#include "common/game/npc/npc.h"
#include "common/persistencia/vinculo_mazmorra.h"

//El id se asigna solo (incremental) para que sea unico dentro del mapa.
struct NpcEditor {
    uint16_t id;
    TipoNpc  tipo;
    uint16_t x;
    uint16_t y;
};

struct CriaturaEditor {
    uint16_t     id;
    TipoCriatura tipo;
    uint16_t     x;
    uint16_t     y;
};

// Elemento sobre el piso (arbol, cartel, etc.)
struct ObjetoEditor {
    std::string clave;
    uint16_t    x;
    uint16_t    y;
};


class EditorMapa {
private:
    uint16_t ancho;
    uint16_t alto;
    uint16_t mapaId;
    std::vector<Posicion> paredes;
    std::vector<Ciudad> ciudades;
    std::vector<NpcEditor> npcs;
    std::vector<CriaturaEditor> criaturas;
    std::vector<ZonaPiso> pisos;  // zonas de piso visual; "ultima gana"
    std::vector<ObjetoEditor> objetos;  // elementos sobre el piso (arboles, carteles)
    std::string pisoBase;
    std::optional<VinculoMazmorra> vinculoMazmorra;
    uint16_t marcadorX;
    uint16_t marcadorY;

    uint16_t proximoIdNpc() const;
    uint16_t proximoIdCriatura() const;

public:
    EditorMapa(uint16_t ancho, uint16_t alto);

    uint16_t getAncho() const;
    uint16_t getAlto() const;
    uint16_t getMapaId() const;
    void setMapaId(uint16_t id);
    const std::string& getPisoBase() const;
    const std::optional<VinculoMazmorra>& getVinculoMazmorra() const;
    bool esMazmorra() const;
    // Celda del portal (entrada/salida) en este mapa: marcador arrastrable.
    uint16_t getMarcadorX() const;
    uint16_t getMarcadorY() const;
    void setMarcador(uint16_t x, uint16_t y);
    bool esMarcador(uint16_t x, uint16_t y) const;

    bool dentroDeLimites(uint16_t x, uint16_t y) const;
    bool hayParedEn(uint16_t x, uint16_t y) const;
    bool hayObjetoEn(uint16_t x, uint16_t y) const;
    bool hayNpcEn(uint16_t x, uint16_t y) const;
    bool hayCriaturaEn(uint16_t x, uint16_t y) const;
    bool estaEnCiudad(uint16_t x, uint16_t y) const;
    bool celdaOcupada(uint16_t x, uint16_t y) const;
    // Celda sin piso pintado: queda como "vacio" (relleno intransitable). Solo se
    // puede pintar piso encima; no admite paredes, objetos, NPCs ni criaturas.
    bool esVacio(uint16_t x, uint16_t y) const;
    std::string pisoEn(uint16_t x, uint16_t y) const;
    bool todoCubierto() const;

    void ponerPared(uint16_t x, uint16_t y);
    void ponerObjeto(const std::string& clave, uint16_t x, uint16_t y);
    void ponerNpc(TipoNpc tipo, uint16_t x, uint16_t y);
    void ponerCriatura(TipoCriatura tipo, uint16_t x, uint16_t y);
    void agregarCiudad(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void pintarParedes(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void pintarPiso(const std::string& clave, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void borrarEn(uint16_t x, uint16_t y);

    const std::vector<Posicion>&       getParedes() const;
    const std::vector<Ciudad>&         getCiudades() const;
    const std::vector<NpcEditor>&      getNpcs() const;
    const std::vector<CriaturaEditor>& getCriaturas() const;
    const std::vector<ZonaPiso>&       getPisos() const;
    const std::vector<ObjetoEditor>&   getObjetos() const;

    void redimensionar(uint16_t nuevoAncho, uint16_t nuevoAlto);

    void cargarDesde(const Mapa& mapa, uint16_t mapaId,
                     const std::optional<VinculoMazmorra>& vinculo);
    Mapa construirMapa() const;


};

#endif
