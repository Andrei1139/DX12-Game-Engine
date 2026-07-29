#pragma once
#include <chrono>
#include <iostream>

using std::chrono::steady_clock;

class Timer {
    public:
        explicit Timer(int fps = 60): fps{fps}, nanosecsPerFrame{nanosecsInOneSec / fps} {
            timerStart = steady_clock::now();
            secondStart = timerStart;
        }
        void setFrameDisplay(bool framerateDisplay) {this->framerateDisplay = framerateDisplay;}
        void endFrame();


    private:
        int fps;
        int framesCounter = 0;
        steady_clock::time_point secondStart, timerStart, timerEnd;

        long long nanosecsInOneSec = 1'000'000'000L;
        long long nanosecsPerFrame;

        bool framerateDisplay = false;
};