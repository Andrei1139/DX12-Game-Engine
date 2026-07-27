#include "OBJLoader.hpp"

static void getTokensInLine(char *line, std::vector<Token> &lineTokens) {
    std::istringstream ISS(line);
    std::string strToken;

    while (ISS >> strToken) {
        lineTokens.push_back(Token{strToken});
    }
}

static std::vector<std::vector<Token>> getTokensInFile(const char *fileName, bool optional = false) {
    std::string filePath("assets/");
    filePath.append(fileName);
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        if (optional) return std::vector<std::vector<Token>>();
        throw std::runtime_error("Failed to load file");
    }

    // Separate words/individual symbols/values into individual tokens
    std::vector<std::vector<Token>> tokens;
    char line[100];
    while(!fileStream.eof()) {
        tokens.push_back(std::vector<Token>());
        
        fileStream.getline(line, 100);
        getTokensInLine(line, tokens.at(tokens.size() - 1));
        
        // Ignore spacing lines
        if (tokens.at(tokens.size() - 1).size() == 0) {
            tokens.pop_back();
        }
    }

    return tokens;
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

Model OBJLoader::loadModel(const char *fileName) {
    // File MUST have .obj extension
    size_t pathLen = strlen(fileName);
    if (strcmp(fileName + pathLen - 4, ".obj") != 0) {
        throw std::runtime_error("Invalid file: format must be .obj");
    }

    auto tokens = getTokensInFile(fileName);
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
            loadModelMaterial(model, line.at(1).str.c_str());
        }
    }

    return model;
}

void OBJLoader::loadModelMaterial(Model &model, const char *fileName) {
    // File MUST have .mtl extension
    size_t pathLen = strlen(fileName);
    if (strcmp(fileName + pathLen - 4, ".mtl") != 0) {
        throw std::runtime_error("Invalid file: format must be .mtl");
    }

    auto tokens = getTokensInFile(fileName, true);
    if (tokens.empty()) return;
    OBJLoader::processModelMaterial(model, tokens);
}

void OBJLoader::processModelMaterial(Model &model, const std::vector<std::vector<Token>> &tokens) {
    for (const std::vector<Token> line: tokens) {
        auto &first = line.at(0);

        if (first.str == "map_Kd") {
            model.setTextureFileName(std::wstring(line.at(1).str.begin(), line.at(1).str.end()));
            return;
        }
    }
}