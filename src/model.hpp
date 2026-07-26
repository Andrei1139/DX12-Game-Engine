#pragma once
#include <vector>
#include "utils.hpp"

class Model {
    public:
        Model &addFace(std::vector<Vertex> vertices) {
            for (int i = 1; i < vertices.size() - 1; ++i) {
                addTriangle(vertices.at(0), vertices.at(i), vertices.at(i + 1));
            }

            return *this;
        }

        Model &addTriangle(Vertex v1, Vertex v2, Vertex v3) {
            addVertex(v1);
            addVertex(v2);
            addVertex(v3);
            
            return *this; // For continuous calling of the function
        }

        const Vertex *getVertices() const {return vertices.data();}
        const uint32_t *getIndices() const {return indices.data();}

        UINT getNumVertices() const {return static_cast<UINT>(vertices.size());}
        UINT getNumIndices() const {return static_cast<UINT>(indices.size());}

        void setTextureFileName(std::wstring pFileName) {texFileName = pFileName;}
        std::wstring getTextureFileName() const {return texFileName;}
    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::wstring texFileName = std::wstring(L"default.png");

        void addVertex(Vertex v) {
            UINT index = getNumVertices();

            // Reuse vertex if already utilised
            for (UINT i = 0; i < getNumVertices(); ++i) {
                if (vertices.at(i) == v) {
                    indices.push_back(i);
                    return;
                }
            }

            vertices.push_back(v);
            indices.push_back(index);
        }
};