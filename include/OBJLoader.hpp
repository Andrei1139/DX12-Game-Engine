#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include "model.hpp"

typedef struct _Token {
    std::string str;
    inline float getFloat() const {return std::stof(str);}
} Token;

class OBJLoader {
    public:
        static Model loadModel(const char *fileName);
    private:
        static Model processModel(const std::vector<std::vector<Token>> &tokens);
        static void loadModelMaterial(Model &model, const char *fileName);
        static void processModelMaterial(Model &model, const std::vector<std::vector<Token>> &tokens);
};