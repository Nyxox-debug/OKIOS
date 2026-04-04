#include "engine/brain.hpp"
#include <cmath>

void Brain::init(std::mt19937 &rng) {
  float rangeIH = xavierRange(5); // fan-in = 5 inputs
  float rangeHO = xavierRange(8); // fan-in = 8 hidden nodes

  std::uniform_real_distribution<float> distIH(-rangeIH, rangeIH);
  std::uniform_real_distribution<float> distHO(-rangeHO, rangeHO);

  for (auto &w : weightsInputHidden)
    w = distIH(rng);
  for (auto &w : weightsHiddenOutput)
    w = distHO(rng);

  biasHidden.fill(0.0f);
  biasOutput.fill(0.0f);
}

output Brain::forward(const input &in) const {
  const std::array<float, 5> inputs = {in.health, in.hunger, in.direction.x,
                                       in.direction.y, in.distance};

  std::array<float, 8> hidden;
  for (int i = 0; i < 8; i++) {
    float sum = biasHidden[i];
    for (int j = 0; j < 5; j++)
      sum += weightsInputHidden[i * 5 + j] * inputs[j];
    hidden[i] = std::tanh(sum);
  }

  std::array<float, 2> out;
  for (int i = 0; i < 2; i++) {
    float sum = biasOutput[i];
    for (int j = 0; j < 8; j++)
      sum += weightsHiddenOutput[i * 8 + j] * hidden[j];
    out[i] = std::tanh(sum);
  }

  return {glm::vec2(out[0], out[1])};
}

Brain Brain::mutate(float mutationRate, std::mt19937 &rng) const {
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
  Brain child = *this;

  for (auto &w : child.weightsInputHidden)
    w += dist(rng) * mutationRate;
  for (auto &w : child.weightsHiddenOutput)
    w += dist(rng) * mutationRate;
  for (auto &b : child.biasHidden)
    b += dist(rng) * mutationRate;
  for (auto &b : child.biasOutput)
    b += dist(rng) * mutationRate;

  return child;
}
