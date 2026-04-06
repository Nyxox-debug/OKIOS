#define STB_PERLIN_IMPLEMENTATION
#include "engine/stb_perlin.h"
#include "engine/terrain.hpp"

Terrain::Terrain(int gridSize, float cellSize, glm::vec2 offset,
                 float noiseOffsetX, float noiseOffsetZ) {
  modelMat = glm::mat4(1.0f);
  modelMat = glm::translate(modelMat, glm::vec3(offset.x, -0.5f, offset.y));
  normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
  this->offset = offset;
  this->cellSize = cellSize;
  this->gridSize = gridSize;
  this->noiseOffsetX = noiseOffsetX;
  this->noiseOffsetZ = noiseOffsetZ;
  generate(gridSize, cellSize);
  bounds = this->getBounds();
}

void Terrain::generate(int gridSize, float cellSize) {
  std::vector<Vertex> verts;
  std::vector<unsigned int> indices;

  for (int z = 0; z <= gridSize; z++) {
    for (int x = 0; x <= gridSize; x++) {
      Vertex v;
      float sine = stb_perlin_noise3(
          (x + noiseOffsetX) * 0.1f, 0, (z + noiseOffsetZ) * 0.1f, 0, 0, 0);
      heights.push_back(sine);
      v.Position = {x * cellSize, sine, z * cellSize};

      if (sine > 0.3f)
        v.Color = {0.7f, 0.7f, 0.7f};
      else if (sine > 0.0f)
        v.Color = {0.4f, 0.4f, 0.4f};
      else
        v.Color = {0.25f, 0.25f, 0.25f};

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
  int idx = cellZ * (gridSize + 1) + cellX;
  if (idx < 0 || idx >= (int)heights.size())
    return 0.0f;
  return heights[idx];
}

// Second independent noise channel — biome is purely terrain-shape-driven.
// Returns [-1, 1]: negative = valley/fertile, positive = highland/rocky.
float Terrain::biomeAt(float x, float z) const {
  return stb_perlin_noise3(
      (x + noiseOffsetX) * 0.1f, 100.0f, (z + noiseOffsetZ) * 0.1f, 0, 0, 0);
}

glm::vec4 Terrain::getBounds() {
  float minX = offset.x;
  float maxX = offset.x + (gridSize * cellSize);
  float minZ = offset.y;
  float maxZ = offset.y + (gridSize * cellSize);
  return glm::vec4{minX, maxX, minZ, maxZ};
}

void Terrain::Draw(Shader &shader) { mesh->Draw(shader); }
