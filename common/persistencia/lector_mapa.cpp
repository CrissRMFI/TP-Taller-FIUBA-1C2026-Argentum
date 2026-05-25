#include "lector_mapa.h"

#include <cstring>
#include <fstream>
#include <ios>
#include <vector>

#include "error_persistencia.h"
#include "formato_mapa.h"
#include "../game/npc/npc.h"

void LectorMapa::leerBloque(std::ifstream& archivo, void* destino, std::size_t bytes, const std::string& path) {

    if (bytes == 0) {
        return;
    }

    archivo.read(reinterpret_cast<char*>(destino), static_cast<std::streamsize>(bytes));
    
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::EOF_PREMATURO, path);
    }
}

MapaCargado LectorMapa::leer(const std::string& path) {
  
    const char magicEsperado[4] = {'A', 'O', 'M', '1'};
    const uint16_t versionMaxima = 1;

    std::ifstream archivo(path, std::ios::binary);
    if (!archivo) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, path);
    }

    HeaderMapa header{};
    leerBloque(archivo, &header, sizeof(header), path);

    if (std::memcmp(header.magic, magicEsperado, sizeof(header.magic)) != 0) {
        throw ErrorPersistencia(CodigoErrorPersistencia::MAGIC_INVALIDO, path);
    }

    if (header.version > versionMaxima) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::VERSION_INCOMPATIBLE,
                path + " (version=" + std::to_string(header.version) + ")");
    }

    if (header.ancho == 0 || header.alto == 0) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::DIMENSIONES_INVALIDAS, path);
    }

    std::vector<ParedRecord> paredes(header.cantParedes);
    leerBloque(archivo, paredes.data(),
               paredes.size() * sizeof(ParedRecord), path);

    std::vector<CiudadRecord> ciudades(header.cantCiudades);
    leerBloque(archivo, ciudades.data(),
               ciudades.size() * sizeof(CiudadRecord), path);

    std::vector<NpcRecord> npcs(header.cantNpcs);
    leerBloque(archivo, npcs.data(),
               npcs.size() * sizeof(NpcRecord), path);

    archivo.peek();
    if (!archivo.eof()) {
        throw ErrorPersistencia(CodigoErrorPersistencia::BYTES_EXTRA, path);
    }

    Mapa mapa(header.ancho, header.alto);

    try {
        for (const ParedRecord& p : paredes) {
            mapa.agregarPared(Posicion{p.x, p.y, header.mapaId});
        }
        for (const CiudadRecord& c : ciudades) {
            mapa.agregarCiudad(Ciudad{header.mapaId, c.xMin, c.yMin, c.xMax, c.yMax});
        }
    } catch (const std::invalid_argument& e) {
        throw ErrorPersistencia(
                CodigoErrorPersistencia::REGISTRO_INVALIDO,
                path + " (" + e.what() + ")");
    }

    for (const NpcRecord& n : npcs) {
        const Npc npc{
            n.id,
            static_cast<TipoNpc>(n.tipo),
            Posicion{n.x, n.y, header.mapaId}
        };
        if (!mapa.agregarNpc(npc)) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::NPC_DUPLICADO_O_INVALIDO,
                    path + " (id=" + std::to_string(n.id) + ")");
        }
    }

    return MapaCargado{std::move(mapa), header.mapaId};
}
