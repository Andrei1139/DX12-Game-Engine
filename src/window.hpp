#pragma once
#include <windows.h>
#include <hidusage.h>
#include "camera.hpp"

class EngineWindow {
    private:
        int width, height;

        LPCSTR windowClassName = "MainWindow";
        HINSTANCE currAppInstanceHandle = GetModuleHandle(NULL);
        WNDCLASS windowClass = {};

        HWND windowHandle;

        RECT windowRect;

        static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam);

        bool open = true;

        void finishFrameChanges();
        float dx = 0, dy = 0;

        Camera &camera;
    public:
        EngineWindow(int width, int height, LPCSTR barName, Camera &camera);
        bool isOpen() const {return open;}
        void handleEvents();
        
        int getWidth() const {return width;}
        int getHeight() const {return height;}
        HWND getWindowHandle() const {return windowHandle;}

        void closeWindow() {open = false; DestroyWindow(windowHandle);}

        void modifyX(int inc) {dx += inc;}
        void modifyY(int inc) {dy += inc;}

        int getXCenter() const {return windowRect.left + (windowRect.right - windowRect.left) / 2;}
        int getYCenter() const {return windowRect.top + (windowRect.bottom - windowRect.top) / 2;}

        bool Won = false, Aon = false, Son = false, Don = false, spaceOn = false, shiftOn = false;
};