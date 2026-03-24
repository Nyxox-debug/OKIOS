#pragma once
#include "mesh.hpp"
#include <memory>

class Terrain {
public:
    Terrain(int gridSize, float cellSize);
    void Draw(Shader& shader);
    glm::mat4 modelMat;
    glm::mat3 normalMat;
    

private:
    std::unique_ptr<Mesh> mesh;
    void generate(int gridSize, float cellSize);
};
