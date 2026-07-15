#pragma once
#include <windows.h>
#include <thread>

class EngineWindow {
    private:
        int width, height;

        LPCSTR windowClassName = "MainWindow";
        HINSTANCE currAppInstanceHandle = GetModuleHandle(NULL);
        WNDCLASS windowClass = {};

        HWND windowHandle;

        static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam);

        std::thread windowThread;
        bool open = true;
    public:
        EngineWindow(int width, int height, LPCSTR barName);
        bool isOpen() const {return open;}
        void handleEvents();
        
        int getWidth() const {return width;}
        int getHeight() const {return height;}
        HWND getWindowHandle() const {return windowHandle;}

        void closeWindow() {open = false; DestroyWindow(windowHandle);}

};