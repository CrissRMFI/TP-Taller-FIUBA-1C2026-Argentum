#include "catalogo_editor.h"

#include <QByteArray>
#include <QFile>
#include <QImage>
#include <QRect>

#include <toml++/toml.hpp>

CatalogoEditor::CatalogoEditor() {
    cargar();
}

QPixmap CatalogoEditor::recortar(const QString& sprite, int x, int y, int w, int h) const {
    QPixmap hoja(sprite);
    if (hoja.isNull()) {
        return hoja;
    }
    const QRect rect(x, y, w, h);
    if (!hoja.rect().contains(rect)) {
        return hoja;
    }
    return hoja.copy(rect);
}

void CatalogoEditor::cargar() {
        QFile archivo("config/criaturas.toml");
    if (!archivo.open(QIODevice::ReadOnly)) {
        return;
    }
    const QByteArray bytes = archivo.readAll();

    toml::table tbl;
    try {
        tbl = toml::parse(std::string_view(bytes.constData(), static_cast<size_t>(bytes.size())));
    } catch (const toml::parse_error&) {
        return;
    }

    const auto leerLista = [this, &tbl](const char* clave, SeccionCatalogo seccion, std::vector<ElementoCatalogo>& destino) {
        const toml::array* lista = tbl[clave].as_array();
        if (lista == nullptr) {
            return;
        }
        for (const toml::node& nodo : *lista) {
            const toml::table* t = nodo.as_table();
            if (t == nullptr) {
                continue;
            }
            ElementoCatalogo elem;
            elem.seccion = seccion;
            elem.clave = QString::fromStdString((*t)["clave"].value_or<std::string>(""));
            elem.nombre = QString::fromStdString((*t)["nombre"].value_or<std::string>(""));
            elem.descripcion =
                    QString::fromStdString((*t)["descripcion"].value_or<std::string>(""));
            if (const auto oro = (*t)["oro"].value<int64_t>()) {
                elem.oro = static_cast<uint32_t>(*oro);
                elem.tieneOro = true;
            }
            const QString sprite =
                    QString::fromStdString((*t)["sprite"].value_or<std::string>(""));
            int x = 0, y = 0, w = 0, h = 0;
            if (const toml::array* src = (*t)["src"].as_array(); src && src->size() == 4) {
                x = static_cast<int>((*src)[0].value_or<int64_t>(0));
                y = static_cast<int>((*src)[1].value_or<int64_t>(0));
                w = static_cast<int>((*src)[2].value_or<int64_t>(0));
                h = static_cast<int>((*src)[3].value_or<int64_t>(0));
            }
            elem.icono = recortar(sprite, x, y, w, h);
            destino.push_back(elem);
        }
    };

    leerLista("criatura", SeccionCatalogo::Criaturas, criaturas);
    leerLista("npc", SeccionCatalogo::Npc, npcs);
}

const std::vector<ElementoCatalogo>& CatalogoEditor::elementosDe(SeccionCatalogo seccion) const {
    switch (seccion) {
        case SeccionCatalogo::Criaturas: return criaturas;
        case SeccionCatalogo::Npc:       return npcs;
        case SeccionCatalogo::Pisos:     return pisos;
    }
    return pisos;
}

bool CatalogoEditor::criaturaPorClave(const QString& clave, TipoCriatura& tipo) const {
    if (clave == "goblin")    { tipo = TipoCriatura::Goblin;    return true; }
    if (clave == "esqueleto") { tipo = TipoCriatura::Esqueleto; return true; }
    if (clave == "zombie")    { tipo = TipoCriatura::Zombie;    return true; }
    if (clave == "arania")    { tipo = TipoCriatura::Arania;    return true; }
    if (clave == "orco")      { tipo = TipoCriatura::Orco;      return true; }
    if (clave == "golem")     { tipo = TipoCriatura::Golem;     return true; }
    return false;
}

bool CatalogoEditor::npcPorClave(const QString& clave, TipoNpc& tipo) const {
    if (clave == "sacerdote")   { tipo = TipoNpc::Sacerdote;   return true; }
    if (clave == "comerciante") { tipo = TipoNpc::Comerciante; return true; }
    if (clave == "banquero")    { tipo = TipoNpc::Banquero;    return true; }
    return false;
}

QPixmap CatalogoEditor::iconoCriatura(TipoCriatura tipo) const {
    TipoCriatura actual;
    for (const ElementoCatalogo& elem : criaturas) {
        if (criaturaPorClave(elem.clave, actual) && actual == tipo) {
            return elem.icono;
        }
    }
    return QPixmap();
}

QPixmap CatalogoEditor::iconoNpc(TipoNpc tipo) const {
    TipoNpc actual;
    for (const ElementoCatalogo& elem : npcs) {
        if (npcPorClave(elem.clave, actual) && actual == tipo) {
            return elem.icono;
        }
    }
    return QPixmap();
}
