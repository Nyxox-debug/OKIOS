#pragma once

#include <memory>
struct GLFWwindow;
class Shader;
class Creature;

class Engine {
private:
  GLFWwindow *window = nullptr;
  std::unique_ptr<Shader> shader;
  std::unique_ptr<Creature> creature;


public:
  Engine();
  ~Engine();
  bool init();
  void run();
  void shutdown();
};
