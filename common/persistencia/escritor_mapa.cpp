#include "escritor_mapa.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <limits>

#include "error_persistencia.h"

uint32_t EscritorMapa::saturarA32(std::size_t valor) {
    if (valor > std::numeric_limits<uint32_t>::max()) {
        throw ErrorPersistencia(CodigoErrorPersistencia::CANTIDAD_EXCEDE_UINT32, "");
    }
    return static_cast<uint32_t>(valor);
}

void EscritorMapa::escribirBloque(std::ofstream& archivo, const void* datos, std::size_t bytes, const std::string& path) {
    
    if (bytes == 0) {
        return;
    }

    archivo.write(reinterpret_cast<const char*>(datos), static_cast<std::streamsize>(bytes));
    
    if (!archivo) {
        throw ErrorPersistencia(CodigoErrorPersistencia::NO_SE_PUEDE_ESCRIBIR, path);
    }
}

void EscritorMapa::agregarRegistroNpc(std::vector<NpcRecord>& destino, const Npc& npc) {
    const Posicion pos = npc.getPosicion();
    destino.push_back(NpcRecord{
        npc.getId(),
        static_cast<uint8_t>(npc.getTipo()),
        /*_padding=*/0,
        pos.x,
        pos.y
    });
}

void EscritorMapa::escribir(const Mapa& mapa, uint16_t mapaId, const std::string& path) {
    const auto& paredes  = mapa.getParedes();
    const auto& ciudades = mapa.getCiudades();

    std::vector<NpcRecord> npcs;
    npcs.reserve(mapa.getSacerdotes().size() + mapa.getComerciantes().size() + mapa.getBanqueros().size());

    for (const auto& [id, npc] : mapa.getSacerdotes()) { 
      agregarRegistroNpc(npcs, npc); 
    }
    
    for (const auto& [id, npc] : mapa.getComerciantes()) { 
      agregarRegistroNpc(npcs, npc); 
    }
    
    for (const auto& [id, npc] : mapa.getBanqueros()) { 
      agregarRegistroNpc(npcs, npc); 
    }

    const char magicEsperado[4] = {
      'A', 'O', 'M', '1'
    };

    const uint16_t versionActual = 1;

    HeaderMapa header{};
    
    std::memcpy(header.magic, magicEsperado, sizeof(header.magic));
    header.version = versionActual;
    header.mapaId = mapaId;
    header.ancho = mapa.getAncho();
    header.alto = mapa.getAlto();
    header.cantParedes = saturarA32(paredes.size());
    header.cantCiudades = saturarA32(ciudades.size());
    header.cantNpcs = saturarA32(npcs.size());

    // tmp + rename: si el proceso muere mientras se escribe, el .bin original
    // (si existia) queda intacto.
    
    const std::string pathTmp = path + ".tmp";

    {
        std::ofstream archivo(pathTmp, std::ios::binary | std::ios::trunc);
        if (!archivo) {
            throw ErrorPersistencia(
                    CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO, pathTmp);
        }

        escribirBloque(archivo, &header, sizeof(header), pathTmp);

        if (!paredes.empty()) {
            std::vector<ParedRecord> paredesRec;
            paredesRec.reserve(paredes.size());
            for (const Posicion& p : paredes) {
                paredesRec.push_back(ParedRecord{p.x, p.y});
            }
            escribirBloque(archivo, paredesRec.data(),
                           paredesRec.size() * sizeof(ParedRecord), pathTmp);
        }

        if (!ciudades.empty()) {
            std::vector<CiudadRecord> ciudadesRec;
            ciudadesRec.reserve(ciudades.size());
            for (const Ciudad& c : ciudades) {
                ciudadesRec.push_back(CiudadRecord{c.xMin, c.yMin, c.xMax, c.yMax});
            }
            escribirBloque(archivo, ciudadesRec.data(),
                           ciudadesRec.size() * sizeof(CiudadRecord), pathTmp);
        }

        escribirBloque(archivo, npcs.data(),
                       npcs.size() * sizeof(NpcRecord), pathTmp);

        archivo.flush();
        if (!archivo) {
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
