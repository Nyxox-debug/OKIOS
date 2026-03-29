#pragma once
#include <array>
#include <glm/glm.hpp>

struct input {
  float health;
  float hunger;
  glm::vec2 direction;
  float distance;
};

struct output {
  glm::vec2 direction; // x and y
};

class Brain {
private:
  std::array<float, 8 * 5> weightsInputHidden;
  std::array<float, 8> biasHidden;
  std::array<float, 2 * 8> weightsHiddenOutput;
  std::array<float, 2> biasOutput;

public:
  output forward(const input &in) const;
  void init();
};
