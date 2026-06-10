#ifndef CATALOGO_EDITOR_H
#define CATALOGO_EDITOR_H

#include <cstdint>
#include <string>
#include <vector>

#include <QPixmap>
#include <QString>

#include "common/game/criatura.h"
#include "common/game/npc/npc.h"

enum class SeccionCatalogo {
    Criaturas,
    Npc,
    Pisos,
};


struct ElementoCatalogo {
    SeccionCatalogo seccion;
    QString clave;
    QString nombre;
    QString descripcion;
    uint32_t oro = 0;
    bool tieneOro = false;
    QPixmap icono;
    QString destino; 
    bool tieneStats = false;
    uint16_t vida = 0;
    uint8_t nivel = 0;
    uint8_t fuerza = 0;
    uint8_t agilidad = 0;
    uint8_t aggro = 0;
    uint8_t danioMin = 0;
    uint8_t danioMax = 0;
};

class CatalogoEditor {
private:
    std::vector<ElementoCatalogo> criaturas;
    std::vector<ElementoCatalogo> npcs;
    std::vector<ElementoCatalogo> pisos;

    void cargar();
    void parsearLista(const std::string& archivoPath, const char* claveArray,
                      SeccionCatalogo seccion, std::vector<ElementoCatalogo>& destino);
    QPixmap recortar(const QString& sprite, int x, int y, int w, int h) const;

public:
    CatalogoEditor();

    const std::vector<ElementoCatalogo>& elementosDe(SeccionCatalogo seccion) const;
    bool criaturaPorClave(const QString& clave, TipoCriatura& tipo) const;
    bool npcPorClave(const QString& clave, TipoNpc& tipo) const;
    
    QPixmap iconoCriatura(TipoCriatura tipo) const;
    QPixmap iconoNpc(TipoNpc tipo) const;
    QPixmap tilePiso(const QString& clave) const;
    QString destinoPiso(const QString& clave) const;


};

#endif
