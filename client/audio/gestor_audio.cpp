#include "gestor_audio.h"

#include <algorithm>
#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include <toml++/toml.hpp>

#include "../../common/mensajes/mensajes_error_audio.h"

// Canales de mezcla simultaneos. Si todos estan ocupados, los efectos nuevos se descartan (anti-saturacion natural cuando hay muchos eventos a la vez).
#define CANALES_MEZCLA 24
#define VOLUMEN_MAX 128

GestorAudio::GestorAudio(const std::string& rutaConfig, const std::string& resourcesRoot):
        audioOk(false),
        subsistemaIniciado(false),
        volumenMaestroPct(90),
        volumenMusicaPct(55),
        radioAudibleCeldas(18) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        std::cerr << "[audio] " << MensajesErrorAudio::mensaje(CodigoErrorAudio::SUBSISTEMA_NO_INICIADO)
                  << ": " << SDL_GetError() << std::endl;
        return;
    }
    subsistemaIniciado = true;

    try {
        mixer = std::make_unique<SDL2pp::Mixer>(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048);
    } catch (const std::exception& e) {
        std::cerr << "[audio] " << MensajesErrorAudio::mensaje(CodigoErrorAudio::DISPOSITIVO_NO_ABIERTO)
                  << ": " << e.what() << std::endl;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        subsistemaIniciado = false;
        return;
    }

    Mix_AllocateChannels(CANALES_MEZCLA);
    cargarCatalogo(rutaConfig, resourcesRoot);
    audioOk = true;
}

GestorAudio::~GestorAudio() {
    musicas.clear();
    efectos.clear();
    mixer.reset();
    if (subsistemaIniciado) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

int GestorAudio::volumenCanalDe(int volumenBase0a100) const {
    const int base = std::clamp(volumenBase0a100, 0, 100);
    return VOLUMEN_MAX * base / 100 * volumenMaestroPct / 100;
}

void GestorAudio::cargarCatalogo(const std::string& rutaConfig, const std::string& resourcesRoot) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(rutaConfig);
    } catch (const toml::parse_error& e) {
        std::cerr << "[audio] " << MensajesErrorAudio::mensaje(CodigoErrorAudio::CONFIG_NO_LEIDA)
                  << " ('" << rutaConfig << "': " << e.description() << ")" << std::endl;
        return;
    }

    volumenMaestroPct = tbl["general"]["volumen_maestro"].value_or(volumenMaestroPct);
    volumenMusicaPct = tbl["general"]["volumen_musica"].value_or(volumenMusicaPct);
    radioAudibleCeldas = tbl["general"]["radio_audible"].value_or(radioAudibleCeldas);
    if (radioAudibleCeldas <= 0) {
        radioAudibleCeldas = 1;
    }

    
    if (const toml::table* efectosTbl = tbl["efectos"].as_table()) {
        for (const auto& [clave, valor] : *efectosTbl) {
            const toml::table* def = valor.as_table();
            if (def == nullptr) {
                continue;
            }
            const std::optional<std::string> path = (*def)["path"].value<std::string>();
            if (!path) {
                continue;
            }
            const int volumen = (*def)["volumen"].value_or(100);
            const std::string nombre(clave.str());
            try {
                efectos[nombre] =
                        std::make_unique<SDL2pp::Chunk>(resourcesRoot + "/" + *path);
                volumenCanal[nombre] = volumenCanalDe(volumen);
            } catch (const std::exception& e) {
                std::cerr << "[audio] " << MensajesErrorAudio::mensaje(CodigoErrorAudio::EFECTO_NO_CARGADO)
                          << " '" << nombre << "' (" << *path << "): " << e.what() << std::endl;
            }
        }
    }

    // Musica: cada subtabla de [musica] con un 'path' es un track.
    if (const toml::table* musicaTbl = tbl["musica"].as_table()) {
        for (const auto& [clave, valor] : *musicaTbl) {
            const toml::table* def = valor.as_table();
            if (def == nullptr) {
                continue;
            }
            const std::optional<std::string> path = (*def)["path"].value<std::string>();
            if (!path) {
                continue;
            }
            const std::string nombre(clave.str());
            try {
                musicas[nombre] =
                        std::make_unique<SDL2pp::Music>(resourcesRoot + "/" + *path);
            } catch (const std::exception& e) {
                std::cerr << "[audio] " << MensajesErrorAudio::mensaje(CodigoErrorAudio::MUSICA_NO_CARGADA)
                          << " '" << nombre << "' (" << *path << "): " << e.what() << std::endl;
            }
        }
    }
}

void GestorAudio::reproducirEfecto(const std::string& clave) {
    if (!audioOk) {
        return;
    }
    const auto it = efectos.find(clave);
    if (it == efectos.end() || !it->second) {
        return;
    }
    try {
        const int canal = mixer->PlayChannel(-1, *it->second, 0);
        mixer->SetVolume(canal, volumenCanal[clave]);
    } catch (const std::exception&) {
        // Sin canal libre: se descarta el efecto (anti-saturacion). No es error.
    }
}

void GestorAudio::reproducirEfectoPosicional(const std::string& clave, int distanciaCeldas) {
    if (!audioOk) {
        return;
    }
    if (distanciaCeldas > radioAudibleCeldas) {
        return;  // fuera del alcance: no suena
    }
    const auto it = efectos.find(clave);
    if (it == efectos.end() || !it->second) {
        return;
    }
    
    const int distancia255 =
            std::clamp(distanciaCeldas * 255 / radioAudibleCeldas, 0, 255);
    try {
        const int canal = mixer->PlayChannel(-1, *it->second, 0);
        mixer->SetVolume(canal, volumenCanal[clave]);
        mixer->SetDistance(canal, static_cast<Uint8>(distancia255));
    } catch (const std::exception&) {
        // Sin canal libre: se descarta.
    }
}

void GestorAudio::reproducirMusica(const std::string& clave) {
    if (!audioOk || clave == musicaActual) {
        return;
    }
    const auto it = musicas.find(clave);
    if (it == musicas.end() || !it->second) {
        return;
    }
    try {
        mixer->SetMusicVolume(VOLUMEN_MAX * std::clamp(volumenMusicaPct, 0, 100) / 100);
        mixer->PlayMusic(*it->second, -1);  // -1 = loop infinito
        musicaActual = clave;
    } catch (const std::exception& e) {
        std::cerr << "[audio] " << MensajesErrorAudio::mensaje(CodigoErrorAudio::MUSICA_NO_REPRODUCIDA)
                  << " '" << clave << "': " << e.what() << std::endl;
    }
}

void GestorAudio::detenerMusica() {
    if (!audioOk) {
        return;
    }
    mixer->HaltMusic();
    musicaActual.clear();
}
