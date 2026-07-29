#include "../include/timer.hpp"

void Timer::endFrame() {
    timerEnd = steady_clock::now();

    while ((timerEnd - timerStart).count() < nanosecsPerFrame) {
        timerEnd = steady_clock::now();
    }

    timerStart = steady_clock::now();
    if (framerateDisplay) {
        ++framesCounter;
        if ((timerStart - secondStart).count() >= nanosecsInOneSec) {
            std::cout << framesCounter << " FPS\n";
            
            framesCounter = 0;
            secondStart = timerStart;
        }
    }
}