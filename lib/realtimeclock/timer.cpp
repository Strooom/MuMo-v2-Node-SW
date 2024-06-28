#include <timer.hpp>
#include <realtimeclock.hpp>

void timer::start(uint32_t newPeriodInSeconds) {
    startTime       = realTimeClock::get();
    periodInSeconds = newPeriodInSeconds;
    running         = true;
}

void timer::stop() {
    running = false;
}

bool timer::expired() {
    if (running) {
        if (realTimeClock::get() > (startTime + periodInSeconds)) {
            startTime += periodInSeconds;
            return true;
        }
    }
    return false;
}
