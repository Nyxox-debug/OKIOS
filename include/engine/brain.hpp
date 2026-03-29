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

struct experience {
  input state;
  output action;
  float reward;
};

class Brain {
private:
  std::array<float, 8 * 5> weightsInputHidden;
  std::array<float, 8> biasHidden;
  std::array<float, 2 * 8> weightsHiddenOutput;
  std::array<float, 2> biasOutput;
  mutable std::array<float, 8> hiddenLayActivations;
  mutable std::array<float, 2> outLayActivations;

public:
  output forward(const input &in) const;
  void backward(const std::vector<experience> &history, float learningRate);
  void init();
};
