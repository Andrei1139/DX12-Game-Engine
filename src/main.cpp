#include <iostream>
#include <chrono>
#include "window.hpp"
#include "graphicsEngine.hpp"

using std::chrono::steady_clock;
constexpr long long fps = 60L;

int main() {
    auto frameEnd = steady_clock::now();
    try {
        EngineWindow window(800, 600, "Game Engine");
        GraphicsEngine graphicsEngine(window);
        int x = 0;
        while (window.isOpen()) {
            auto frameStart = frameEnd;

            window.handleEvents();
            graphicsEngine.render();
            graphicsEngine.finishFrame();

            graphicsEngine.printHFAILEDoutput();

            frameEnd = steady_clock::now();
            std::this_thread::sleep_for(std::chrono::nanoseconds(1'000'000'000L / fps) - (frameEnd - frameStart));
        }
    } catch (const std::exception& exception) {
        MessageBoxA(NULL, exception.what(), "Exception", MB_ICONERROR | MB_OK);
    }
}