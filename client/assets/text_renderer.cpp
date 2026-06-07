#include "text_renderer.h"

#include "SDL2pp/Point.hh"
#include "SDL2pp/Surface.hh"
#include "SDL2pp/Texture.hh"

TextRenderer::TextRenderer(const std::string& ruta_fuente, const int tam):
        ttf(), font(ruta_fuente, tam) {}

void TextRenderer::dibujar(SDL2pp::Renderer& renderer, const std::string& texto, const int x,
                           const int y, const SDL_Color& color) {
    if (texto.empty()) {
        return;
    }
    
    SDL2pp::Surface surface = font.RenderText_Blended(texto, color);
    SDL2pp::Texture texture(renderer, surface);
    renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Point(x, y));
}

int TextRenderer::alto_linea() const {
    return font.GetLineSkip();
}
