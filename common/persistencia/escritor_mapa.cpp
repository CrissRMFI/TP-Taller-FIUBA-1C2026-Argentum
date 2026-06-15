#include "escritor_mapa.h"

#include <cstdio>
#include <fstream>
#include <ios>
#include <ostream>

#include "error_persistencia.h"
#include "firma_mapa.h"

const char* EscritorMapa::tipoNpcATexto(TipoNpc tipo) {
    switch (tipo) {
        case TipoNpc::Banquero:    return "banquero";
        case TipoNpc::Comerciante: return "comerciante";
        case TipoNpc::Sacerdote:   return "sacerdote";
    }
    return "desconocido";
}

const char* EscritorMapa::tipoCriaturaATexto(TipoCriatura tipo) {
    switch (tipo) {
        case TipoCriatura::Goblin:    return "goblin";
        case TipoCriatura::Esqueleto: return "esqueleto";
        case TipoCriatura::Zombie:    return "zombie";
        case TipoCriatura::Arania:    return "arania";
        case TipoCriatura::Orco:      return "orco";
        case TipoCriatura::Golem:     return "golem";
        case TipoCriatura::CentinelaPiedra:   return "centinela_piedra";
        case TipoCriatura::GuerreroAncestral: return "guerrero_ancestral";
        case TipoCriatura::Aberracion:        return "aberracion";
        case TipoCriatura::ColosoRoca:        return "coloso_roca";
        case TipoCriatura::SenorAbismo:       return "senor_abismo";
    }
    return "desconocido";
}

void EscritorMapa::escribirCriaturas(std::ostream& out, const Mapa& mapa) {
    for (const Criatura& criatura : mapa.obtenerCriaturas()) {
        const Posicion pos = criatura.getPos();
        out << "  { id = " << criatura.getId()
            << ", tipo = \"" << tipoCriaturaATexto(criatura.getTipo()) << "\""
            << ", x = " << pos.x
            << ", y = " << pos.y << " },\n";
    }
}

void EscritorMapa::escribirNpcs(std::ostream& out, const Mapa& mapa) {
    const auto emitir = [this, &out](const auto& mapaNpcs) {
        for (const auto& [id, npc] : mapaNpcs) {
            const Posicion pos = npc.getPosicion();
            out << "  { id = " << npc.getId()
                << ", tipo = \"" << tipoNpcATexto(npc.getTipo()) << "\""
                << ", x = " << pos.x
                << ", y = " << pos.y << " },\n";
        }
    };
    emitir(mapa.getSacerdotes());
    emitir(mapa.getComerciantes());
    emitir(mapa.getBanqueros());
}

void EscritorMapa::escribir(const Mapa& mapa, uint16_t mapaId, const std::string& path) {
    // tmp + rename: si el proceso muere mientras se escribe, el archivo previo (si existia) queda intacto.
    const std::string pathTmp = path + ".tmp";

    {
        std::ofstream out(pathTmp, std::ios::trunc);
        if (!out) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, pathTmp);
        }

        out << "# config/mapa.toml - escenario de Argentum (paredes, "
               "ciudades/zonas seguras y NPCs).\n"
               "# Generado por el editor de mapas. Editable a mano.\n\n";

        // Firma del formato: identifica el archivo como un mapa de Argentum.
        out << ARGENTUM_MAPA_FIRMA_CLAVE " = \"" ARGENTUM_MAPA_FIRMA "\"\n";
        out << "mapa_id = " << mapaId << "\n";
        out << "ancho   = " << mapa.getAncho() << "\n";
        out << "alto    = " << mapa.getAlto() << "\n\n";

        out << "# Celdas bloqueadas (no transitables).\n";
        out << "paredes = [\n";
        for (const Posicion& p : mapa.getParedes()) {
            out << "  { x = " << p.x << ", y = " << p.y << " },\n";
        }
        out << "]\n\n";

        out << "# Ciudades / zonas seguras: rectangulos "
               "[x_min..x_max] x [y_min..y_max].\n";
        out << "ciudades = [\n";
        for (const Ciudad& c : mapa.getCiudades()) {
            out << "  { x_min = " << c.xMin << ", y_min = " << c.yMin
                << ", x_max = " << c.xMax << ", y_max = " << c.yMax << " },\n";
        }
        out << "]\n\n";

        out << "# NPCs: tipo = \"banquero\" | \"comerciante\" | \"sacerdote\".\n";
        out << "npcs = [\n";
        escribirNpcs(out, mapa);
        out << "]\n\n";

        out << "# Criaturas: tipo = \"goblin\" | \"esqueleto\" | \"zombie\" | "
               "\"arania\" | \"orco\" | \"golem\".\n";
        out << "criaturas = [\n";
        escribirCriaturas(out, mapa);
        out << "]\n\n";

        out << "# Pisos: zonas de terreno visual (clave segun config/pisos.toml).\n";
        out << "# Se resuelven por celda 'ultima gana'; sin zona, la celda es pasto.\n";
        out << "pisos = [\n";
        for (const ZonaPiso& p : mapa.getPisos()) {
            out << "  { clave = \"" << p.clave << "\""
                << ", x_min = " << p.xMin << ", y_min = " << p.yMin
                << ", x_max = " << p.xMax << ", y_max = " << p.yMax << " },\n";
        }
        out << "]\n\n";

        out << "# Objetos: elementos sobre el piso (clave segun config/elementos.toml).\n";
        out << "# Ocupan una celda y bloquean el paso (arboles, carteles, etc.).\n";
        out << "objetos = [\n";
        for (const ObjetoMapa& o : mapa.getObjetos()) {
            out << "  { clave = \"" << o.clave << "\""
                << ", x = " << o.x << ", y = " << o.y << " },\n";
        }
        out << "]\n";

        out.flush();
        if (!out) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::NO_SE_PUEDE_ESCRIBIR, pathTmp);
        }
    }

    if (std::rename(pathTmp.c_str(), path.c_str()) != 0) {
        std::remove(pathTmp.c_str());
        throw ErrorPersistencia(
                CodigoErrorPersistencia::NO_SE_PUEDE_RENOMBRAR, path);
    }
}
