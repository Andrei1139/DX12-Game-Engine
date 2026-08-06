cmake_minimum_required(VERSION 3.10.0)
project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

add_executable(GameEngine src/main.cpp
    src/window.cpp
    src/graphicsEngine.cpp
    src/resourceManager.cpp
    src/OBJLoader.cpp
    src/model.cpp
    src/timer.cpp
    src/interface.cpp
    src/messageQueue.cpp)

target_link_libraries(GameEngine PRIVATE d3d12.lib dxgi.lib d3dcompiler.lib DirectXTK12 runtimeobject.lib)

add_subdirectory(${CMAKE_SOURCE_DIR}/libs/DirectXTK12 ${CMAKE_BINARY_DIR}/bin/CMake/DirectXTK12)


