#pragma once
#include "camera.hpp"
#include "world.hpp"
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;
class Shader;

class Engine {
private:
  GLFWwindow *window   = nullptr;
  bool        running  = false;
  float       deltaTime = 0.0f;
  float       lastFrame = 0.0f;

  std::unique_ptr<Shader> shader;
  std::unique_ptr<Camera> camera;

  void processInput();

public:
  World world;

  Engine();
  ~Engine();

  bool init();
  void run();
  void update(float dt);
  void shutdown();

  // Phase 15
  // Save the current population brains to disk.
  void savePopulation(const std::string &path);

  // Clear current population, swap to a new terrain seed, reload saved brains.
  void loadAndTransfer(const std::string &path, float newNoiseOffsetX,
                       float newNoiseOffsetZ);
};
