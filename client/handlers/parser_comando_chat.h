#ifndef PARSER_COMANDO_CHAT_H
#define PARSER_COMANDO_CHAT_H

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../common/protocolo/comando_jugador.h"

// Resultado de interpretar una linea escrita en el mini-chat.
//  - comando: presente si la linea produjo un comando valido para enviar.
//  - error:   texto a mostrar al jugador si la linea era invalida.
// Si ambos quedan vacios, la linea no produjo nada (p.ej. linea en blanco).
struct ResultadoParseo {
    std::optional<ComandoJugador> comando;
    std::string error;
};

/* Convierte el texto que el jugador escribe en el mini-chat en un ComandoJugador. Es logica pura: no depende de SDL ni de la red. El mapeo nombre-de-objeto -> id se inyecta por constructor (se carga desde el TOML de configuracion, eso nos evita el hardcodeo).
*/
class ParserComandoChat {
public:
    explicit ParserComandoChat(std::unordered_map<std::string, uint16_t> nombreItemAId);

    ResultadoParseo parsear(const std::string& linea,
                            std::optional<uint16_t> objetivoSeleccionado) const;

private:
    std::unordered_map<std::string, uint16_t> nombreItemAId;

    ResultadoParseo parsearChatPrivado(const std::string& linea) const;
    ResultadoParseo parsearComando(const std::string& cuerpo,
                                   std::optional<uint16_t> objetivoSeleccionado) const;

    ResultadoParseo parsearDepositar(const std::vector<std::string>& tokens,
                                     std::optional<uint16_t> objetivo) const;
    ResultadoParseo parsearRetirar(const std::vector<std::string>& tokens,
                                   std::optional<uint16_t> objetivo) const;
    ResultadoParseo parsearGestionClan(Opcode opcode,
                                       const std::vector<std::string>& tokens) const;

    std::string aMinusculas(const std::string& texto) const;
    std::string recortar(const std::string& texto) const;
    std::vector<std::string> tokenizar(const std::string& texto) const;
    std::string restoDeLinea(const std::string& linea, size_t desdeToken) const;
    std::string restoCrudo(const std::string& cuerpo) const;
    std::optional<uint16_t> idDeItem(const std::string& nombre) const;
    std::optional<uint32_t> aEntero(const std::string& texto) const;

    ResultadoParseo soloComando(Opcode opcode, PayloadComando payload) const;
    ResultadoParseo conError(const std::string& mensaje) const;
};

#endif
