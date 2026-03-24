#include "engine/terrain.hpp"
#include <glm/glm.hpp>

Terrain::Terrain(int gridSize, float cellSize, glm::vec2 offset) {
  modelMat = glm::mat4(1.0f);
  modelMat = glm::translate(modelMat, glm::vec3(offset.x, -0.5f, offset.y));
  normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
  this->offset = offset;
  this->cellSize = cellSize;
  generate(gridSize, cellSize);
}

void Terrain::generate(int gridSize, float cellSize) {
  std::vector<Vertex> verts;
  std::vector<unsigned int> indices;

  for (int z = 0; z <= gridSize; z++) {
    for (int x = 0; x <= gridSize; x++) {
      Vertex v;
      v.Position = {x * cellSize, 0.0f, z * cellSize};
      v.Color = {0.2f, 0.5f, 0.2f};
      verts.push_back(v);
    }
  }

  for (int z = 0; z < gridSize; z++) {
    for (int x = 0; x < gridSize; x++) {
      int row = z * (gridSize + 1);
      indices.push_back(row + x);
      indices.push_back(row + x + (gridSize + 1));
      indices.push_back(row + x + 1);
      indices.push_back(row + x + 1);
      indices.push_back(row + x + (gridSize + 1));
      indices.push_back(row + x + (gridSize + 1) + 1);
    }
  }

  mesh = std::make_unique<Mesh>(verts, indices);
}

float Terrain::terrainHeight(float x, float z) {
  glm::vec2 localSpace = glm::vec2(x - offset.x, z - offset.y);
  int cellX = (int)(localSpace.x / cellSize);
  int cellZ = (int)(localSpace.y / cellSize);
  // TODO: return the right height when mountains and hills are added 
  return 0.0;
};

void Terrain::Draw(Shader &shader) { mesh->Draw(shader); }
