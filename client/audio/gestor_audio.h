#ifndef GESTOR_AUDIO_H
#define GESTOR_AUDIO_H

#include <memory>
#include <string>
#include <unordered_map>

#include <SDL2pp/Chunk.hh>
#include <SDL2pp/Mixer.hh>
#include <SDL2pp/Music.hh>

/*
Esta clase e spara reproducir el sonido, consumido desde el TOML
(resources/config/sonidos.toml)
*/

class GestorAudio {

private:
    bool audioOk;
    bool subsistemaIniciado;
    std::unique_ptr<SDL2pp::Mixer> mixer;

    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Chunk>> efectos;
    std::unordered_map<std::string, int> volumenCanal;
    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Music>> musicas;

    int volumenMaestroPct;
    int volumenMusicaPct;
    int radioAudibleCeldas;
    std::string musicaActual;

    // Canal dedicado para el loop de pasos (-1 si no esta sonando).
    int canalPasos;
    bool pasosActivos;

    void cargarCatalogo(const std::string& rutaConfig, const std::string& resourcesRoot);
    int volumenCanalDe(int volumenBase0a100) const;

public:
    
    GestorAudio(const std::string& rutaConfig, const std::string& resourcesRoot);
    ~GestorAudio();

    GestorAudio(const GestorAudio&) = delete;
    GestorAudio& operator=(const GestorAudio&) = delete;

    // Efecto no posicional suena a su volumen base.
    void reproducirEfecto(const std::string& clave);

    // Efecto posicional: el volumen se atenua segun la distancia (en celdas)
    void reproducirEfectoPosicional(const std::string& clave, int distanciaCeldas);

    // Musica de fondo en loop.
    void reproducirMusica(const std::string& clave);
    void detenerMusica();

    // Loop de pasos en un canal dedicado: arranca al empezar a caminar y se corta al detenerse
    void reproducirPasos();
    void detenerPasos();

    bool disponible() const { return audioOk; }

};

#endif
