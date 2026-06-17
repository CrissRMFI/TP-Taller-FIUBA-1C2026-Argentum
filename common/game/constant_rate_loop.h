//
// Created by victoria zubieta on 15/06/2026.
//

#ifndef TALLER_TP_CONSTANT_RATE_LOOP_H
#define TALLER_TP_CONSTANT_RATE_LOOP_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>

class ConstantRateLoop {
private:
    std::chrono::milliseconds tick;
    std::atomic_bool running;

public:
    explicit ConstantRateLoop(std::chrono::milliseconds tick);

    void stop();

    void run(const std::function<void(uint32_t ticks_elapsed, uint64_t tick_number)>& on_tick);
};

#endif  // TALLER_TP_CONSTANT_RATE_LOOP_H
