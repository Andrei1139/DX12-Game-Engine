#include <iostream>
#include <thread>
#include "../include/timer.hpp"
#include "../include/camera.hpp"
#include "../include/window.hpp"
#include "../include/graphicsEngine.hpp"
#include "../include/interface.hpp"

int main() {
    // Needed for Windows Imagining Component functionality
    #if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize))
        return -1;
    #else
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        // error
    #endif

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    Interface _interface;

    // std::thread interfaceThread(&Interface::display, &_interface);

    try {
        Camera camera(width, height);
        EngineWindow window(width, height, "Game Engine", camera);
        GraphicsEngine graphicsEngine(window, camera);

        Timer timer(FPS);
        timer.setFrameDisplay(true);

        Object object;
        object.setScaleX(0.5f).setScaleY(0.5f).setScaleZ(0.5f);
        graphicsEngine.addObject(object, "cube.obj");

        graphicsEngine.finishInitialization();
        
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