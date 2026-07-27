#pragma once
#include "object.hpp"

class Scene {
    public:
        void addObject(Object &object) {
            objects.push_back(object);
        }
    private:
        std::vector<Object> objects;
};