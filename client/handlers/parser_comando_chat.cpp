#include "parser_comando_chat.h"

#include <algorithm>
#include <cctype>
#include <sstream>

ParserComandoChat::ParserComandoChat(std::unordered_map<std::string, uint16_t> nombreItemAId):
        nombreItemAId(std::move(nombreItemAId)) {}

ResultadoParseo ParserComandoChat::parsear(const std::string& linea,
                                           std::optional<uint16_t> objetivoSeleccionado) const {
    const std::string l = recortar(linea);
    if (l.empty()) {
        return {};
    }
    if (l[0] == '@') {
        return parsearChatPrivado(l);
    }
    if (l[0] != '/') {
        return soloComando(Opcode::CHAT_GLOBAL, ComandoChatGlobal{l});
    }
    return parsearComando(l.substr(1), objetivoSeleccionado);
}

ResultadoParseo ParserComandoChat::parsearChatPrivado(const std::string& linea) const {
    // Formato: @<nick> <mensaje>
    const size_t espacio = linea.find_first_of(" \t");
    if (espacio == std::string::npos) {
        return conError("Uso: @<nick> <mensaje>");
    }
    const std::string nick = linea.substr(1, espacio - 1);
    const std::string mensaje = recortar(linea.substr(espacio + 1));
    if (nick.empty() || mensaje.empty()) {
        return conError("Uso: @<nick> <mensaje>");
    }
    return soloComando(Opcode::CHAT_PRIVADO, ComandoChatPrivado{nick, mensaje});
}

ResultadoParseo ParserComandoChat::parsearComando(const std::string& cuerpo,
                                                  std::optional<uint16_t> objetivo) const {
    const std::vector<std::string> tokens = tokenizar(cuerpo);
    if (tokens.empty()) {
        return conError("Comando vacio");
    }
    const std::string cmd = aMinusculas(tokens[0]);

    // --- Comandos sin argumentos ---
    if (cmd == "meditar") {
        return soloComando(Opcode::MEDITAR, ComandoMeditar{});
    }
    if (cmd == "resucitar") {
        return soloComando(Opcode::RESUCITAR, ComandoResucitar{});
    }

    // --- Comandos que requieren un NPC/jugador seleccionado ---
    if (cmd == "curar") {
        if (!objetivo) {
            return conError("Selecciona un sacerdote con click primero");
        }
        return soloComando(Opcode::CURAR, ComandoCurar{*objetivo});
    }
    if (cmd == "listar") {
        if (!objetivo) {
            return conError("Selecciona un comerciante o banquero con click primero");
        }
        return soloComando(Opcode::LISTAR, ComandoListar{*objetivo});
    }
    if (cmd == "comprar") {
        if (tokens.size() < 2) {
            return conError("Uso: /comprar <objeto>");
        }
        if (!objetivo) {
            return conError("Selecciona un comerciante o sacerdote con click primero");
        }
        const auto idItem = idDeItem(restoDeLinea(cuerpo, 1));
        if (!idItem) {
            return conError("Objeto desconocido: " + restoDeLinea(cuerpo, 1));
        }
        return soloComando(Opcode::COMPRAR, ComandoComprar{*idItem, *objetivo});
    }
    if (cmd == "vender") {
        if (tokens.size() < 2) {
            return conError("Uso: /vender <slot-inventario>");
        }
        if (!objetivo) {
            return conError("Selecciona un comerciante con click primero");
        }
        const auto slot = aEntero(tokens[1]);
        if (!slot || *slot > 255) {
            return conError("Slot de inventario invalido");
        }
        return soloComando(Opcode::VENDER,
                           ComandoVender{static_cast<uint8_t>(*slot), *objetivo});
    }
    if (cmd == "depositar") {
        return parsearDepositar(tokens, objetivo);
    }
    if (cmd == "retirar") {
        return parsearRetirar(tokens, objetivo);
    }

    // --- Inventario propio ---
    if (cmd == "tirar") {
        if (tokens.size() < 2) {
            return conError("Uso: /tirar <slot-inventario>");
        }
        const auto slot = aEntero(tokens[1]);
        if (!slot || *slot > 255) {
            return conError("Slot de inventario invalido");
        }
        return soloComando(Opcode::TIRAR, ComandoTirar{static_cast<uint8_t>(*slot)});
    }
    if (cmd == "equipar") {
        if (tokens.size() < 2) {
            return conError("Uso: /equipar <slot-inventario>");
        }
        const auto slot = aEntero(tokens[1]);
        if (!slot || *slot > 255) {
            return conError("Slot de inventario invalido");
        }
        return soloComando(Opcode::EQUIPAR, ComandoEquipar{static_cast<uint8_t>(*slot)});
    }

    return conError("Comando desconocido: /" + cmd);
}

ResultadoParseo ParserComandoChat::parsearDepositar(const std::vector<std::string>& tokens,
                                                    std::optional<uint16_t> objetivo) const {
    if (!objetivo) {
        return conError("Selecciona un banquero con click primero");
    }
    if (tokens.size() >= 3 && aMinusculas(tokens[1]) == "oro") {
        const auto monto = aEntero(tokens[2]);
        if (!monto) {
            return conError("Monto de oro invalido");
        }
        return soloComando(Opcode::DEPOSITAR_ORO, ComandoDepositarOro{*monto, *objetivo});
    }
    if (tokens.size() >= 2) {
        const auto slot = aEntero(tokens[1]);
        if (!slot || *slot > 255) {
            return conError("Slot de inventario invalido");
        }
        return soloComando(Opcode::DEPOSITAR_ITEM,
                           ComandoDepositarItem{static_cast<uint8_t>(*slot), *objetivo});
    }
    return conError("Uso: /depositar <slot-inventario>  |  /depositar oro <cant>");
}

ResultadoParseo ParserComandoChat::parsearRetirar(const std::vector<std::string>& tokens,
                                                  std::optional<uint16_t> objetivo) const {
    if (!objetivo) {
        return conError("Selecciona un banquero con click primero");
    }
    if (tokens.size() >= 3 && aMinusculas(tokens[1]) == "oro") {
        const auto monto = aEntero(tokens[2]);
        if (!monto) {
            return conError("Monto de oro invalido");
        }
        return soloComando(Opcode::RETIRAR_ORO, ComandoRetirarOro{*monto, *objetivo});
    }
    if (tokens.size() >= 2) {
        // El retiro de un item se referencia por nombre (el banco indexa por id).
        std::string nombre;
        for (size_t i = 1; i < tokens.size(); ++i) {
            if (i > 1) {
                nombre += '_';
            }
            nombre += tokens[i];
        }
        const auto idItem = idDeItem(nombre);
        if (!idItem) {
            return conError("Objeto desconocido: " + nombre);
        }
        return soloComando(Opcode::RETIRAR_ITEM, ComandoRetirarItem{*idItem, *objetivo});
    }
    return conError("Uso: /retirar <objeto>  |  /retirar oro <cant>");
}

std::optional<uint16_t> ParserComandoChat::idDeItem(const std::string& nombre) const {
    const auto it = nombreItemAId.find(aMinusculas(nombre));
    if (it == nombreItemAId.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<uint32_t> ParserComandoChat::aEntero(const std::string& texto) const {
    if (texto.empty()) {
        return std::nullopt;
    }
    for (const char c : texto) {
        if (std::isdigit(static_cast<unsigned char>(c)) == 0) {
            return std::nullopt;
        }
    }
    try {
        return static_cast<uint32_t>(std::stoul(texto));
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string ParserComandoChat::aMinusculas(const std::string& texto) const {
    std::string r = texto;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return r;
}

std::string ParserComandoChat::recortar(const std::string& texto) const {
    const size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) {
        return "";
    }
    const size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

std::vector<std::string> ParserComandoChat::tokenizar(const std::string& texto) const {
    std::vector<std::string> tokens;
    std::istringstream iss(texto);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string ParserComandoChat::restoDeLinea(const std::string& linea, size_t desdeToken) const {
    // Devuelve el texto a partir del token numero 'desdeToken' (0-based),
    // normalizado: minusculas y con espacios internos reemplazados por '_'
    // para casar con las claves del catalogo (p.ej. "arco simple" -> "arco_simple").
    const std::vector<std::string> tokens = tokenizar(linea);
    std::string resto;
    for (size_t i = desdeToken; i < tokens.size(); ++i) {
        if (i > desdeToken) {
            resto += '_';
        }
        resto += tokens[i];
    }
    return aMinusculas(resto);
}

ResultadoParseo ParserComandoChat::soloComando(Opcode opcode, PayloadComando payload) const {
    return ResultadoParseo{ComandoJugador{opcode, std::move(payload)}, ""};
}

ResultadoParseo ParserComandoChat::conError(const std::string& mensaje) const {
    return ResultadoParseo{std::nullopt, mensaje};
}
