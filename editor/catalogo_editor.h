#ifndef CATALOGO_EDITOR_H
#define CATALOGO_EDITOR_H

#include <cstdint>
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
};

class CatalogoEditor {
private:
    std::vector<ElementoCatalogo> criaturas;
    std::vector<ElementoCatalogo> npcs;
    std::vector<ElementoCatalogo> pisos;

    void cargar();
    QPixmap recortar(const QString& sprite, int x, int y, int w, int h) const;

public:
    CatalogoEditor();

    const std::vector<ElementoCatalogo>& elementosDe(SeccionCatalogo seccion) const;
    bool criaturaPorClave(const QString& clave, TipoCriatura& tipo) const;
    bool npcPorClave(const QString& clave, TipoNpc& tipo) const;
    
    QPixmap iconoCriatura(TipoCriatura tipo) const;
    QPixmap iconoNpc(TipoNpc tipo) const;


};

#endif
