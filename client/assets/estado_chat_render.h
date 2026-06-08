#ifndef TALLER_TP_ESTADO_CHAT_RENDER_H
#define TALLER_TP_ESTADO_CHAT_RENDER_H

#include <string>
#include <utility>
#include <vector>

#include <SDL_pixels.h>

// Tipo de linea de chat: define el color (centralizado, configurable por TOML).
enum class TipoMensajeChat {
    Normal,        // blanco
    Ataque,        // me atacan -> rojo
    Hechizo,       // lanzo un hechizo -> naranja
    Sistema,       // "no hay nada aqui", etc. -> celeste oscuro
    Experiencia,   // gano XP -> azul
    Recuperacion,  // recupero vida/mana -> verde oscuro
    Clan,          // mensajes con @ -> marron
    CriticoHecho,      // yo pego un critico -> dorado
    CriticoRecibido,   // me pegan un critico -> magenta
};

using LineaChat = std::pair<std::string, TipoMensajeChat>;

/* Configuracion ESTATICA del chat: sale del TOML y no cambia entre frames. Se le pasa al renderer una sola vez en init(). El ancho de la caja no esta aca: lo deriva el
renderer como la mitad del ancho de la ventana.
*/
struct ConfigChatRender {
    std::string fuenteRuta;
    int         fuenteTam = 14;
    std::string fondoRuta;
    int         panelX = 0;
    int         panelY = 0;
    int         panelAlto = 0;
    SDL_Color   colorTexto = {255, 255, 255, 255};
    SDL_Color   colorInput = {255, 255, 0, 255};
    // Colores por tipo de mensaje (configurables en el TOML).
    SDL_Color   colorAtaque = {220, 60, 60, 255};
    SDL_Color   colorHechizo = {240, 150, 40, 255};
    SDL_Color   colorSistema = {40, 120, 160, 255};
    SDL_Color   colorExperiencia = {80, 120, 255, 255};
    SDL_Color   colorRecuperacion = {40, 150, 60, 255};
    SDL_Color   colorClan = {150, 100, 50, 255};
    SDL_Color   colorCriticoHecho = {255, 200, 60, 255};      // dorado
    SDL_Color   colorCriticoRecibido = {255, 40, 100, 255};   // magenta

    SDL_Color colorPara(TipoMensajeChat tipo) const {
        switch (tipo) {
            case TipoMensajeChat::Ataque:          return colorAtaque;
            case TipoMensajeChat::Hechizo:         return colorHechizo;
            case TipoMensajeChat::Sistema:         return colorSistema;
            case TipoMensajeChat::Experiencia:     return colorExperiencia;
            case TipoMensajeChat::Recuperacion:    return colorRecuperacion;
            case TipoMensajeChat::Clan:            return colorClan;
            case TipoMensajeChat::CriticoHecho:    return colorCriticoHecho;
            case TipoMensajeChat::CriticoRecibido: return colorCriticoRecibido;
            default:                               return colorTexto;
        }
    }
};

// Estado DINAMICO del chat en un frame: lo arma el loop a partir del input handler y del world. Es lo unico que cambia frame a frame.
struct EstadoChatRender {
    bool activo = false; // hay foco para escribir
    std::string entrada; // texto que se esta tipeando
    std::vector<LineaChat> historial; // mensajes entrantes (mas viejo -> mas nuevo) con su tipo
};

#endif
