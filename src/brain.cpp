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

  for (int i = 0; i < 8; i++) {
    float sum = biasHidden[i];
    for (int j = 0; j < 5; j++) {
      sum += weightsInputHidden[i * 5 + j] * inputs[j];
    }
    hiddenLayActivations[i] = tanh(sum);
  }

  for (int i = 0; i < 2; i++) {
    float sum = biasOutput[i];
    for (int j = 0; j < 8; j++) {
      sum += weightsHiddenOutput[i * 8 + j] * hiddenLayActivations[j];
    }
    outLayActivations[i] = tanh(sum);
  }

  return {glm::vec2(outLayActivations[0], outLayActivations[1])};
}

// TODO: I don't actually get this part, I used AI for the backward function
void Brain::backward(const std::vector<experience> &history,
                     float learningRate) {
  float gamma = 0.99f;
  std::vector<float> G(history.size(), 0.0f);

  // work backwards
  G[history.size() - 1] = history.back().reward;
  for (int t = history.size() - 2; t >= 0; t--) {
    G[t] = history[t].reward + gamma * G[t + 1];
  }

  for (int t = 0; t < history.size(); t++) {
    forward(history[t].state); // repopulates hiddenLayActivations

    // output layer error
    for (int i = 0; i < 2; i++) {
      float delta = G[t] * (1.0f - outLayActivations[i] * outLayActivations[i]);
      for (int j = 0; j < 8; j++) {
        weightsHiddenOutput[i * 8 + j] +=
            learningRate * delta * hiddenLayActivations[j];
      }
      biasOutput[i] += learningRate * delta;
    }

    // hidden layer error
    for (int i = 0; i < 8; i++) {
      float error = 0.0f;
      for (int k = 0; k < 2; k++) {
        float delta =
            G[t] * (1.0f - outLayActivations[k] * outLayActivations[k]);
        error += delta * weightsHiddenOutput[k * 8 + i];
      }
      float hiddenDelta =
          error * (1.0f - hiddenLayActivations[i] * hiddenLayActivations[i]);
      std::array<float, 5> inputs = {
          history[t].state.health, history[t].state.hunger,
          history[t].state.direction.x, history[t].state.direction.y,
          history[t].state.distance};
      for (int j = 0; j < 5; j++) {
        weightsInputHidden[i * 5 + j] += learningRate * hiddenDelta * inputs[j];
      }
      biasHidden[i] += learningRate * hiddenDelta;
    }
  }
}

Brain Brain::mutate(float mutationRate) const {
  Brain child = *this;

  for (int i = 0; i < child.weightsInputHidden.size(); i++) {
    child.weightsInputHidden[i] +=
        ((rand() % 200) / 100.0f - 1.0f) * mutationRate;
  }
  for (int i = 0; i < child.weightsHiddenOutput.size(); i++) {
    child.weightsHiddenOutput[i] +=
        ((rand() % 200) / 100.0f - 1.0f) * mutationRate;
  }
  for (int i = 0; i < child.biasHidden.size(); i++) {
    child.biasHidden[i] += ((rand() % 200) / 100.0f - 1.0f) * mutationRate;
  }
  for (int i = 0; i < child.biasOutput.size(); i++) {
    child.biasOutput[i] += ((rand() % 200) / 100.0f - 1.0f) * mutationRate;
  }

  return child;
}
