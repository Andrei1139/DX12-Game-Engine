#pragma once
#include <vector>
#include "utils.hpp"

class Model {
    public:
        Model(std::initializer_list<Vertex> verticesList, std::initializer_list<uint32_t> indicesList):
        vertices{verticesList}, indices{indicesList} {}

        const Vertex *getVertices() const {return vertices.data();}
        const uint32_t *getIndices() const {return indices.data();}

        size_t getNumVertices() const {return vertices.size();}
        size_t getNumIndices() const {return indices.size();}
    private:
        const std::vector<Vertex> vertices;
        const std::vector<uint32_t> indices;
};