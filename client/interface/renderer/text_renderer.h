#ifndef TALLER_TP_TEXT_RENDERER_H
#define TALLER_TP_TEXT_RENDERER_H

#include <string>

#include <SDL_pixels.h>

#include "SDL2pp/Font.hh"
#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDLTTF.hh"

class TextRenderer {
private:
    SDL2pp::SDLTTF ttf;
    SDL2pp::Font font;

public:
    TextRenderer(const std::string& ruta_fuente, int tam);

    void dibujar(SDL2pp::Renderer& renderer, const std::string& texto, int x, int y,
                 const SDL_Color& color);

    // Dibuja el texto escalado por 'escala' (para titulos grandes como el nombre del jugador).
    void dibujarEscalado(SDL2pp::Renderer& renderer, const std::string& texto, int x, int y,
                         const SDL_Color& color, float escala);

    int alto_linea() const;
};

#endif
