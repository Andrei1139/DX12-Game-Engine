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
        case WM_INPUT: {
            static UINT rawInputDataSize = sizeof(RAWINPUT);
            static RAWINPUT rawInputData;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (void *)&rawInputData, &rawInputDataSize, sizeof(RAWINPUTHEADER));

            if (rawInputData.header.dwType == RIM_TYPEMOUSE) {
                int xPos = rawInputData.data.mouse.lLastX;
                int yPos = rawInputData.data.mouse.lLastY;

                engineWindow->modifyX(xPos);
                engineWindow->modifyY(yPos);
            }

            break;
        }
        case WM_KEYDOWN: {
            switch (wParam) {
                case 'W':
                    engineWindow->Won = true;
                    break;
                case 'A':
                    engineWindow->Aon = true;
                    break;
                case 'S':
                    engineWindow->Son = true;
                    break;
                case 'D':
                    engineWindow->Don = true;
                    break;
                case VK_SPACE:
                    engineWindow->spaceOn = true;
                    break;
                case VK_SHIFT:
                    engineWindow->shiftOn = true;
                    break;
            }
            
            return 0;
        }
        case WM_KEYUP: {
            switch (wParam) {
                case 'W':
                    engineWindow->Won = false;
                    break;
                case 'A':
                    engineWindow->Aon = false;
                    break;
                case 'S':
                    engineWindow->Son = false;
                    break;
                case 'D':
                    engineWindow->Don = false;
                    break;
                case VK_SPACE:
                    engineWindow->spaceOn = false;
                    break;
                case VK_SHIFT:
                    engineWindow->shiftOn = false;
                    break;

                case 'X':
                    engineWindow->closeWindow();
                    return 0;
            }
            
            return 0;
        }
        case WM_CLOSE:
            engineWindow->closeWindow();
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(windowHandle, msgCode, wParam, lParam);
}

EngineWindow::EngineWindow(int width, int height, LPCSTR barName, Camera &camera): camera{camera} {
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

    // Registering the Human Interface Device for high-precision mouse input detection
    RAWINPUTDEVICE rawInputDevice = {};
    rawInputDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
    rawInputDevice.usUsage = HID_USAGE_GENERIC_MOUSE;
    rawInputDevice.dwFlags = RIDEV_INPUTSINK;
    rawInputDevice.hwndTarget = windowHandle;
    RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));

    GetWindowRect(windowHandle, &windowRect);

    ShowCursor(false);
    ShowWindow(windowHandle, SW_SHOWDEFAULT);
}

void EngineWindow::handleEvents() {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
        do {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0);
    }

    SetCursorPos(getXCenter(), getYCenter());
    finishFrameChanges();
}

void EngineWindow::finishFrameChanges() {
    if (!(dx == 0 && dy == 0)) {
        camera.rotateCamera(dx, dy);
        dx = 0;
        dy = 0;
    }

    float moveDx = 0, moveDy = 0, moveDz = 0;
    if (Won) {
        ++moveDz;
    }
    if (Aon) {
        --moveDx;
    }
    if (Son) {
        --moveDz;
    }
    if (Don) {
        ++moveDx;
    }
    if (spaceOn) {
        ++moveDy;
    }
    if (shiftOn) {
        --moveDy;
    }

    camera.move(moveDx, moveDy, moveDz);
}