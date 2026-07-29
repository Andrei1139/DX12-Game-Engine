#include "../include/model.hpp"

Model &Model::addFace(std::vector<Vertex> vertices) {
    // Convert face 0,1,...n unto triangles 0,1,2 ; 0,2,3 ;...; 0,n-1,n
    for (int i = 1; i < vertices.size() - 1; ++i) {
        addTriangle(vertices.at(0), vertices.at(i), vertices.at(i + 1));
    }
    sections.at(sections.size() - 1).endTriangleIndex += vertices.size() - 2;

    return *this;
}

Model &Model::addTriangle(Vertex v1, Vertex v2, Vertex v3) {
    addVertex(v1);
    addVertex(v2);
    addVertex(v3);
    
    return *this; // For continuous calling of the function
}

void Model::addSection(std::string material) {
    // Section has not been modified -> no need for new sectioning
    if (sections.at(0).endTriangleIndex == 0) {
        sections.at(0).materialName = material;
    } else {
        ModelSection section;
        section.materialName = material;
        section.beginTriangleIndex = section.endTriangleIndex = sections.at(sections.size() - 1).endTriangleIndex;
        sections.push_back(section);
    }
}

void Model::setRGB(float r, float g, float b, std::string material) {
    for (auto &section: sections) {
        if (!material.empty() && section.materialName == material) {
            section.diffuseR = r;
            section.diffuseG = g;
            section.diffuseB = b;
        }
    }
}

void Model::setTexture(std::wstring textureFileName, std::string material) {
    for (auto &section: sections) {
        if (!material.empty() && section.materialName == material) {
            section.textureFileName = textureFileName;
        }
    }
}

void Model::addVertex(Vertex v) {
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