#ifndef CATEGORIA_CHAT_H
#define CATEGORIA_CHAT_H

#include <cstdint>

enum class CategoriaChat : uint8_t {
    Normal = 0,   // chat global
    Privado = 1,  // chat privado (@)
    Sistema = 2,  // aviso del juego sin emisor
};

#endif
