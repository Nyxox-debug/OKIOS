#pragma once
#include "camera.h"
#include <memory>
struct GLFWwindow;
class Shader;
class Creature;

class Engine {
private:
  GLFWwindow *window = nullptr;
  bool running = false;
  void processInput();
  std::unique_ptr<Shader> shader;
  std::unique_ptr<Creature> creature;

  std::unique_ptr<Camera> camera;
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

public:
  Engine();
  ~Engine();
  bool init();
  void run();
  void shutdown();
};
