#ifndef POSICION_H
#define POSICION_H

#include <cmath>
#include <cstdint>

struct Posicion {
    uint16_t x;
    uint16_t y;
    uint16_t mapaId;

    bool operator==(const Posicion& otro) const {
        return x == otro.x && y == otro.y && mapaId == otro.mapaId;
    }

    bool operator!=(const Posicion& otro) const { return !(*this == otro); }

    bool mismaMapa(const Posicion& otro) const { return mapaId == otro.mapaId; }

    
    float distanciaEuclidea(const Posicion& otro) const {
        int dx = (int)(x) - (int)(otro.x);
        int dy = (int)(y) - (int)(otro.y);
        return std::sqrt((float)(dx * dx + dy * dy));
    }

    int distanciaManhattan(const Posicion& otro) const {
        return std::abs((int)(x) - (int)(otro.x)) +
               std::abs((int)(y) - (int)(otro.y));
    }

    bool esAdyacente(const Posicion& otro) const {
        return mismaMapa(otro) && distanciaManhattan(otro) == 1;
    }
};

#endif
