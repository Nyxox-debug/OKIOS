#pragma once
#include <array>
#include <random>
#include <vector>
#include <glm/glm.hpp>

struct input {
    float health;
    float hunger;
    glm::vec2 direction;
    float distance;
};

struct output {
    glm::vec2 direction;
};

struct experience {
    input state;
    output action;
    float reward;
};

class Brain {
public:
    std::array<float, 40> weightsInputHidden; // 5 inputs * 8 hidden
    std::array<float, 16> weightsHiddenOutput; // 8 hidden * 2 outputs
    std::array<float, 8>  biasHidden;
    std::array<float, 2>  biasOutput;

    void init(std::mt19937& rng);
    output forward(const input& in) const;
    Brain mutate(float mutationRate, std::mt19937& rng) const;

private:
    // Xavier uniform range for a layer
    static float xavierRange(int fanIn) {
        return std::sqrt(1.0f / fanIn);
    }
};
