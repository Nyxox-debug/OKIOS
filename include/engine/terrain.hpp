#pragma once
#include "creature/mesh.hpp"
#include <memory>

class Terrain {
public:
    Terrain(int gridSize, float cellSize);
    void Draw(Shader& shader);

private:
    std::unique_ptr<Mesh> mesh;
    void generate(int gridSize, float cellSize);
};
