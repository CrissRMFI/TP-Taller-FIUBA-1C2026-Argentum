#include "constant_rate_loop.h"

#include <stdexcept>
#include <thread>

ConstantRateLoop::ConstantRateLoop(const std::chrono::milliseconds tick) :
        tick(tick), running(true) {
    if (tick.count() <= 0) {
        throw std::invalid_argument("El tick del constant rate loop debe ser mayor a cero");
    }
}

void ConstantRateLoop::stop() {
    running = false;
}

// ticks elapsed --> cuantos ticks pasaron
// tick_number --> en que tick estoy (cuantos ticks ya fueron procesados en el ultimo loop)
void ConstantRateLoop::run(
        const std::function<void(uint32_t ticks_elapsed, uint64_t tick_number)>& on_tick) {
    using Clock = std::chrono::steady_clock;

    running = true;
    auto next_tick = Clock::now() + tick;
    uint64_t tick_number = 0;

    while (running) {
        const auto now = Clock::now();
        uint32_t ticks_elapsed = 1;

        if (now > next_tick) {
            const auto lag = now - next_tick;
            ticks_elapsed += static_cast<uint32_t>(lag / tick);
        }

        on_tick(ticks_elapsed, tick_number);
        tick_number += ticks_elapsed;
        next_tick += tick * ticks_elapsed;

        if (!running) {
            break;
        }

        std::this_thread::sleep_until(next_tick);
    }
}
