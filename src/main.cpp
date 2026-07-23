#include <iostream>
#include "timer.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "graphicsEngine.hpp"

int main() {
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    // int width = 800;
    // int height = 600;

    try {
        Camera camera(width, height);
        EngineWindow window(width, height, "Game Engine", camera);
        GraphicsEngine graphicsEngine(window, camera);

        Timer timer(FPS);
        timer.setFrameDisplay(true);

        while (window.isOpen()) {
            window.handleEvents();

            graphicsEngine.update();
            graphicsEngine.render();
            graphicsEngine.finishFrame();

            timer.endFrame();
        }
    } catch (const std::exception& exception) {
        MessageBoxA(NULL, exception.what(), "Exception", MB_ICONERROR | MB_OK);
    }
}