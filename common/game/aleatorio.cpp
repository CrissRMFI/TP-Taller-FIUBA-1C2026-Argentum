#include "aleatorio.h"

Aleatorio::Aleatorio(uint64_t semilla) : motor(semilla) {}

Aleatorio::Aleatorio() : motor(std::random_device{}()) {}

float Aleatorio::uniforme() {
    return std::uniform_real_distribution<float>(0.0f, 1.0f)(motor);
}
