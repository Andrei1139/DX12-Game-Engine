#include <iostream>
#include "window.hpp"

LRESULT CALLBACK EngineWindow::WindowProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) {
    static EngineWindow *engineWindow = nullptr; // For reference since the function cannot be set as instance method
    
    switch (msgCode) {
        case WM_NCCREATE: {
            CREATESTRUCT *createStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
            engineWindow = reinterpret_cast<EngineWindow *>(createStruct->lpCreateParams);

            if (!engineWindow) {
                std::cerr << "Cannot access engine window\n";
            }
        }
            break;
        case WM_CLOSE:
            engineWindow->closeWindow();
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(windowHandle, msgCode, wParam, lParam);
}

EngineWindow::EngineWindow(int width, int height, LPCSTR barName) {
    this->width = width;
    this->height = height;

    HINSTANCE currAppInstanceHandle = GetModuleHandle(NULL);
    WNDCLASS windowClass = {};

    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = currAppInstanceHandle;
    windowClass.lpszClassName = windowClassName;
    windowClass.hCursor = LoadCursor(NULL, IDC_CROSS);

    RegisterClass(&windowClass);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    windowHandle = CreateWindowEx(
        0,
        windowClassName,
        barName,
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        (screenWidth - width) / 2, (screenHeight - height) / 2 , width , height,
        NULL, NULL,
        currAppInstanceHandle,
        this
    );

    if (windowHandle == 0) {
        throw std::runtime_error("CreateWindowEx failed\n");
    }

    SetWindowLongPtrA(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(windowHandle, SW_SHOWDEFAULT);
}

void EngineWindow::handleEvents() {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}