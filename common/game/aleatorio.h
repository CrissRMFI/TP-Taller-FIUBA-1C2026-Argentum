#ifndef ALEATORIO_H
#define ALEATORIO_H

#include <cstdint>
#include <random>

// Centralizador del azar del juego. Una sola instancia vive en `Juego` y se
// inyecta a todos los sitios que necesitan muestras aleatorias: cálculo de
// daño, esquive, crítico, movimiento y spawn de criaturas, drops, XP por
// kill, etc.
class Aleatorio {
public:
    explicit Aleatorio(uint64_t semilla);
    Aleatorio();

    // Float uniformemente distribuido en [0, 1).
    float uniforme();

    // Entero uniformemente distribuido en [min, max] (ambos inclusive).
    template <typename T>
    T enteroEnRango(T min, T max) {
        return std::uniform_int_distribution<T>(min, max)(motor);
    }

private:
    std::mt19937_64 motor;
};

#endif
