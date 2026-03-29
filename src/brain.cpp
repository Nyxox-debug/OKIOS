#include "engine/brain.hpp"
#include <cstdlib>

void Brain::init() {
  for (int i = 0; i < weightsInputHidden.size(); i++) {
    weightsInputHidden[i] = ((rand() % 200) / 100.0f) - 1.0f;
  }

  for (int i = 0; i < weightsHiddenOutput.size(); i++) {
    weightsHiddenOutput[i] = ((rand() % 200) / 100.0f) - 1.0f;
  }

  for (int i = 0; i < biasHidden.size(); i++) {
    biasHidden[i] = ((rand() % 200) / 100.0f) - 1.0f;
  }

  for (int i = 0; i < biasOutput.size(); i++) {
    biasOutput[i] = ((rand() % 200) / 100.0f) - 1.0f;
  }
};

output Brain::forward(const input &in) const {
  std::array<float, 5> inputs = {in.health, in.hunger, in.direction.x,
                                 in.direction.y, in.distance};

  std::array<float, 8> hidden;
  for (int i = 0; i < 8; i++) {
    float sum = biasHidden[i];
    for (int j = 0; j < 5; j++) {
      sum += weightsInputHidden[i * 5 + j] * inputs[j];
    }
    hidden[i] = tanh(sum);
  }

  std::array<float, 2> output;
  for (int i = 0; i < 2; i++) {
    float sum = biasOutput[i];
    for (int j = 0; j < 8; j++) {
      sum += weightsHiddenOutput[i * 8 + j] * hidden[j];
    }
    output[i] = tanh(sum);
  }

  return {glm::vec2(output[0], output[1])};
}
