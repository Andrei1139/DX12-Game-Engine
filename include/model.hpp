#pragma once
#include <vector>
#include "utils.hpp"


class Model {
    public:
        class ModelSection {
            public:
                size_t beginTriangleIndex = 0, endTriangleIndex = 0;
                std::string materialName = std::string("");
                std::wstring textureFileName = std::wstring(L"default.png");
                float diffuseR = 1.0f, diffuseG = 1.0f, diffuseB = 1.0f;
        };    
    
        Model &addFace(std::vector<Vertex> vertices);
        Model &addTriangle(Vertex v1, Vertex v2, Vertex v3);

        void addSection(std::string material);

        void setRGB(float r, float g, float b, std::string material);
        void setTexture(std::wstring textureFileName, std::string material);

        const Vertex *getVertices() const {return vertices.data();}
        const uint32_t *getIndices() const {return indices.data();}
        const std::vector<ModelSection> getSections() const {return sections;}

        UINT getNumVertices() const {return static_cast<UINT>(vertices.size());}
        UINT getNumIndices() const {return static_cast<UINT>(indices.size());}

    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<ModelSection> sections = std::vector<ModelSection>(1);

        void addVertex(Vertex v);
};