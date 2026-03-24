#include "engine/terrain.hpp"
#include <glm/glm.hpp>

Terrain::Terrain(int gridSize, float cellSize) {
  modelMat = glm::mat4(1.0f);
  modelMat = glm::translate(modelMat, glm::vec3(-25.0f, -0.5f, -25.0f));
  normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
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

void Terrain::Draw(Shader &shader) { mesh->Draw(shader); }
