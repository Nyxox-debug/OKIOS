#include "engine/brain.hpp"
#include <cmath>
#include <fstream>
#include <stdexcept>

void Brain::init(std::mt19937 &rng) {
  float rangeIH = xavierRange(5);
  float rangeHO = xavierRange(8);
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

// Binary format: just raw floats in order.
// weightsInputHidden (40) + weightsHiddenOutput (16) + biasHidden (8) + biasOutput (2)
// Total: 66 floats = 264 bytes.
void Brain::save(const std::string &path) const {
  std::ofstream f(path, std::ios::binary);
  if (!f)
    throw std::runtime_error("Brain::save — cannot open: " + path);
  f.write(reinterpret_cast<const char *>(weightsInputHidden.data()),
          weightsInputHidden.size() * sizeof(float));
  f.write(reinterpret_cast<const char *>(weightsHiddenOutput.data()),
          weightsHiddenOutput.size() * sizeof(float));
  f.write(reinterpret_cast<const char *>(biasHidden.data()),
          biasHidden.size() * sizeof(float));
  f.write(reinterpret_cast<const char *>(biasOutput.data()),
          biasOutput.size() * sizeof(float));
}

Brain Brain::load(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f)
    throw std::runtime_error("Brain::load — cannot open: " + path);
  Brain b;
  f.read(reinterpret_cast<char *>(b.weightsInputHidden.data()),
         b.weightsInputHidden.size() * sizeof(float));
  f.read(reinterpret_cast<char *>(b.weightsHiddenOutput.data()),
         b.weightsHiddenOutput.size() * sizeof(float));
  f.read(reinterpret_cast<char *>(b.biasHidden.data()),
         b.biasHidden.size() * sizeof(float));
  f.read(reinterpret_cast<char *>(b.biasOutput.data()),
         b.biasOutput.size() * sizeof(float));
  return b;
}
