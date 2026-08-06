#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include "scene.hpp"

class Interface {
    public:
        void display();
        void chooseScene();
        void chooseModel();
    private:
        enum class State {
            MAIN, 
        };
        std::vector<Scene *> scenes;
};