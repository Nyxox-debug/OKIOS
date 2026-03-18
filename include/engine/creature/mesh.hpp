#pragma once
#include "../shader/shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 Position;
};

class Mesh {
private:
  unsigned int VAO, VBO, EBO;
  unsigned int indexCount; // NOTE: Needed for glDrawElements
  void setupMesh();

public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies);
  void Draw(Shader &shader) const;
};
