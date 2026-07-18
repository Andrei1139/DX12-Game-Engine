#include <iostream>
#include <chrono>
#include <thread>
#include "camera.hpp"
#include "window.hpp"
#include "graphicsEngine.hpp"

using std::chrono::steady_clock;
constexpr long long fps = 60L;

int main() {
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    // int width = 800;
    // int height = 600;
    auto frameEnd = steady_clock::now();
    try {
        Camera camera(width, height);
        EngineWindow window(width, height, "Game Engine", camera);
        GraphicsEngine graphicsEngine(window, camera);

        while (window.isOpen()) {
            auto frameStart = steady_clock::now();

            window.handleEvents();

            graphicsEngine.update();
            graphicsEngine.render();
            graphicsEngine.finishFrame();

            frameEnd = steady_clock::now();
            std::this_thread::sleep_for(std::chrono::nanoseconds(1'000'000'000L / fps) - (frameEnd - frameStart));
        }
    } catch (const std::exception& exception) {
        MessageBoxA(NULL, exception.what(), "Exception", MB_ICONERROR | MB_OK);
    }
}