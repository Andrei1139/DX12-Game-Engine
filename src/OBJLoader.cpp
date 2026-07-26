#include "OBJLoader.hpp"

static void getTokens(char *line, std::vector<Token> &lineTokens) {
    std::istringstream ISS(line);
    std::string strToken;

    while (ISS >> strToken) {
        lineTokens.push_back(Token{strToken});
    }
}

static Vertex processIndexedVertex(const Token &token, std::vector<Position> &vertexPos,
                                                       std::vector<Tex> &vertexTex,
                                                       std::vector<Normal> &vertexNormals) {

    UINT posIndex = 0, texIndex = 0, normalIndex = 0;
    std::istringstream str(token.str);

    char aux;
    str >> posIndex;
    if (!str.eof()) {
        str >> aux;
        str >> texIndex;
        if (!str.fail()) {
            str >> aux;
            str >> normalIndex;
        }
    }

    Vertex vertex;
    vertex.pos = (posIndex > -1) ? vertexPos.at(vertexPos.size() + posIndex) : vertexPos.at(posIndex - 1);
    if (texIndex == 0) {
        vertex.tex.u = 0.0f;
        vertex.tex.v = 0.0f;
    } else {
        vertex.tex = (texIndex > -1) ? vertexTex.at(vertexTex.size() + texIndex) : vertexTex.at(texIndex - 1);
    }
     
    return vertex;
}

Model OBJLoader::loadModel(const char *filePath) {
    // File MUST have .obj extension
    size_t pathLen = strlen(filePath);
    if (strcmp(filePath + pathLen - 4, ".obj") != 0) {
        throw std::runtime_error("Invalid file: format must be .obj");
    }

    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Failed to load 3D model");
    }

    // Separate words/individual symbols/values into individual tokens
    std::vector<std::vector<Token>> tokens;
    char line[100];
    while(!fileStream.eof()) {
        tokens.push_back(std::vector<Token>());
        
        fileStream.getline(line, 100);
        getTokens(line, tokens.at(tokens.size() - 1));
        
        // Ignore spacing lines
        if (tokens.at(tokens.size() - 1).size() == 0) {
            tokens.pop_back();
        }
    }

    return OBJLoader::processModel(tokens);
}

Model OBJLoader::processModel(const std::vector<std::vector<Token>> &tokens) {
    Model model;
    std::vector<Position> vertexPos;
    std::vector<Tex> vertexTex;
    std::vector<Normal> vertexNormals;

    for (const std::vector<Token> &line: tokens) {
        auto &first = line.at(0);

        if (first.str == "v") { // Position coords
            Position pos = {line.at(1).getFloat(), line.at(2).getFloat(), line.at(3).getFloat()};
            vertexPos.push_back(pos);
        } else if (first.str == "vt") { // Texture coords
            Tex tex;
            tex.u = line.at(1).getFloat();
            tex.v = (line.size() > 2) ? line.at(2).getFloat() : 0.0f;
            vertexTex.push_back(tex);
        } else if (first.str == "vn") { // Normal vector
            Normal normal = {line.at(1).getFloat(), line.at(2).getFloat(), line.at(3).getFloat()};
            vertexNormals.push_back(normal);
        } else if (first.str == "f") { // Face polygon (triangle or with more sides)
            std::vector<Vertex> vertices;
            for (size_t i = 1; i < line.size(); ++i) {
                vertices.push_back(processIndexedVertex(line.at(i), vertexPos, vertexTex, vertexNormals));
            }

            model.addFace(vertices);
        } else if (first.str == "mtllib") {
            // TODO
        }
    }

    return model;
}