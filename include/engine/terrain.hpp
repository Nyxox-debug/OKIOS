#pragma once
#include "mesh.hpp"
#include <memory>
#include <vector>

class Terrain {
public:
  Terrain(int gridSize, float cellSize, glm::vec2 offset, float noiseOffsetX = 0.0f, float noiseOffsetZ = 0.0f);
  void Draw(Shader &shader);
  float terrainHeight(float x, float z);
  float biomeAt(float x, float z) const;

  glm::vec2 offset;
  float cellSize;
  float gridSize;
  float noiseOffsetX;
  float noiseOffsetZ;
  glm::mat4 modelMat;
  glm::mat3 normalMat;
  glm::vec4 getBounds();
  glm::vec4 bounds;
  std::vector<float> heights;

private:
  std::unique_ptr<Mesh> mesh;
  void generate(int gridSize, float cellSize);
};
