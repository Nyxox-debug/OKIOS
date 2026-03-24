#pragma once
#include "mesh.hpp"
#include <memory>

class Terrain {
public:
  Terrain(int gridSize, float cellSize, glm::vec2 offset);
  void Draw(Shader &shader);
  float terrainHeight(float x, float z);
  glm::vec2 offset;
  float cellSize;
  glm::mat4 modelMat;
  glm::mat3 normalMat;

private:
  std::unique_ptr<Mesh> mesh;
  void generate(int gridSize, float cellSize);
};
