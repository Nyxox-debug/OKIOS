#define STB_PERLIN_IMPLEMENTATION
#include "engine/stb_perlin.h"

#include "engine/terrain.hpp"
// #include <cmath>

Terrain::Terrain(int gridSize, float cellSize, glm::vec2 offset) {
  modelMat = glm::mat4(1.0f);
  modelMat = glm::translate(modelMat, glm::vec3(offset.x, -0.5f, offset.y));
  normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
  this->offset = offset;
  this->cellSize = cellSize;
  this->gridSize = gridSize;
  generate(gridSize, cellSize);
  bounds = this->getBounds();
}

void Terrain::generate(int gridSize, float cellSize) {
  std::vector<Vertex> verts;
  std::vector<unsigned int> indices;

  for (int z = 0; z <= gridSize; z++) {
    for (int x = 0; x <= gridSize; x++) {
      Vertex v;
      // float sine = sin(x * cellSize * 0.3f);
      float sine = stb_perlin_noise3(x * 0.1f, 0, z * 0.1f, 0, 0, 0);
      heights.push_back(sine);
      v.Position = {x * cellSize, sine, z * cellSize};
      if (sine > 0.3f)
        v.Color = {0.5f, 0.5f, 0.5f}; // rocky high ground
      else if (sine > 0.0f)
        v.Color = {0.2f, 0.5f, 0.2f}; // normal grass
      else
        v.Color = {0.1f, 0.3f, 0.6f}; // low/water areas
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

  float l = cellZ * (gridSize + 1) + cellX;
  return heights[l];
};

glm::vec4 Terrain::getBounds() {
  float minX = offset.x;
  float maxX = offset.x + (gridSize * cellSize);

  float minZ = offset.y;
  float maxZ = offset.y + (gridSize * cellSize);

  return glm::vec4{minX, maxX, minZ, maxZ};
};

void Terrain::Draw(Shader &shader) { mesh->Draw(shader); }
