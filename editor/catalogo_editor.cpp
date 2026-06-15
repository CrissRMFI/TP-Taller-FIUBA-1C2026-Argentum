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
    if (w <= 0 || h <= 0) {
        return hoja;
    }
    const QRect rect(x, y, w, h);
    if (!hoja.rect().contains(rect)) {
        return hoja;
    }
    return hoja.copy(rect);
}

void CatalogoEditor::parsearLista(const std::string& archivoPath, const char* claveArray, SeccionCatalogo seccion, std::vector<ElementoCatalogo>& destino) {
    QFile archivo(QString::fromStdString(archivoPath));
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

    const toml::array* lista = tbl[claveArray].as_array();
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
        elem.destino = QString::fromStdString((*t)["destino"].value_or<std::string>(""));
        elem.bloquea = (*t)["bloquea"].value_or<bool>(false);
        if (const toml::array* permitidos = (*t)["pisos_permitidos"].as_array()) {
            for (const toml::node& piso : *permitidos) {
                if (const auto clave = piso.value<std::string>()) {
                    elem.pisosPermitidos.push_back(QString::fromStdString(*clave));
                }
            }
        }
        if (const auto oro = (*t)["oro"].value<int64_t>()) {
            elem.oro = static_cast<uint32_t>(*oro);
            elem.tieneOro = true;
        }
        if (const auto vida = (*t)["vida_maxima"].value<int64_t>()) {
            elem.tieneStats = true;
            elem.vida = static_cast<uint16_t>(*vida);
            elem.nivel = static_cast<uint8_t>((*t)["nivel"].value_or<int64_t>(0));
            elem.fuerza = static_cast<uint8_t>((*t)["fuerza"].value_or<int64_t>(0));
            elem.agilidad = static_cast<uint8_t>((*t)["agilidad"].value_or<int64_t>(0));
            elem.aggro = static_cast<uint8_t>((*t)["rango_aggro"].value_or<int64_t>(0));
            elem.danioMin = static_cast<uint8_t>((*t)["danio_min"].value_or<int64_t>(0));
            elem.danioMax = static_cast<uint8_t>((*t)["danio_max"].value_or<int64_t>(0));
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
}

void CatalogoEditor::cargar() {
    // Criaturas y NPCs: fuente unica compartida con el server
    parsearLista("config/criaturas.toml", "criatura", SeccionCatalogo::Criaturas, criaturas);
    parsearLista("config/criaturas.toml", "npc", SeccionCatalogo::Npc, npcs);
    // Pisos / terreno: catalogo del editor
    parsearLista("config/pisos.toml", "piso", SeccionCatalogo::Pisos, pisos);
    // Elementos (objetos sobre el piso): catalogo del editor
    parsearLista("config/elementos.toml", "elemento", SeccionCatalogo::Elementos, elementos);
}

const std::vector<ElementoCatalogo>& CatalogoEditor::elementosDe(SeccionCatalogo seccion) const {
    switch (seccion) {
        case SeccionCatalogo::Criaturas: return criaturas;
        case SeccionCatalogo::Npc:       return npcs;
        case SeccionCatalogo::Pisos:     return pisos;
        case SeccionCatalogo::Elementos: return elementos;
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
    if (clave == "centinela_piedra")   { tipo = TipoCriatura::CentinelaPiedra;   return true; }
    if (clave == "guerrero_ancestral") { tipo = TipoCriatura::GuerreroAncestral; return true; }
    if (clave == "aberracion")         { tipo = TipoCriatura::Aberracion;        return true; }
    if (clave == "coloso_roca")        { tipo = TipoCriatura::ColosoRoca;        return true; }
    if (clave == "senor_abismo")       { tipo = TipoCriatura::SenorAbismo;       return true; }
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

QPixmap CatalogoEditor::tilePiso(const QString& clave) const {
    for (const ElementoCatalogo& elem : pisos) {
        if (elem.clave == clave) {
            return elem.icono;
        }
    }
    return QPixmap();
}

QString CatalogoEditor::destinoPiso(const QString& clave) const {
    for (const ElementoCatalogo& elem : pisos) {
        if (elem.clave == clave) {
            return elem.destino;
        }
    }
    return QString();
}

bool CatalogoEditor::elementoPorClave(const QString& clave, ElementoCatalogo& out) const {
    for (const ElementoCatalogo& elem : elementos) {
        if (elem.clave == clave) {
            out = elem;
            return true;
        }
    }
    return false;
}

QPixmap CatalogoEditor::iconoElemento(const QString& clave) const {
    for (const ElementoCatalogo& elem : elementos) {
        if (elem.clave == clave) {
            return elem.icono;
        }
    }
    return QPixmap();
}

bool CatalogoEditor::pisoPermitido(const QString& claveElemento, const QString& clavePiso) const {
    ElementoCatalogo elem;
    if (!elementoPorClave(claveElemento, elem)) {
        return false;
    }
    if (elem.pisosPermitidos.empty()) {
        return true;
    }
    for (const QString& permitido : elem.pisosPermitidos) {
        if (permitido == clavePiso) {
            return true;
        }
    }
    return false;
}
