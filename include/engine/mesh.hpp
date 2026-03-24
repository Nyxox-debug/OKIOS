#pragma once
#include "shader/shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Color;
  glm::vec3 normal;
};

class Mesh {
private:
  unsigned int VAO, VBO, EBO;
  void setupMesh();

public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
  void Draw(Shader &shader) const;
};
