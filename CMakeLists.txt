cmake_minimum_required(VERSION 3.10.0)
project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(GameEngine src/main.cpp
    src/window.cpp
    src/graphicsEngine.cpp)

target_link_libraries(GameEngine d3d12.lib dxgi.lib d3dcompiler.lib)

