#include "engine/engine.hpp"
#include "engine/creature/creature.hpp"
#include <iostream>
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float vertices[] = {
    // positions        // colors
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // top right, red
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right, green
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left, blue
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f  // top left, yellow
};

unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first Triangle
    1, 2, 3  // second Triangle
};

Engine::Engine() = default;
Engine::~Engine() = default;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

bool Engine::init() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  shader = std::make_unique<Shader>("../res/shaders/def.vert",
                                    "../res/shaders/def.frag");
  running = true;
  return true;
};

void Engine::run() {
  if (!running)
    return;

  std::vector<Vertex> vertices = {
      {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},   // top right, red
      {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // bottom right, green
      {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // bottom left, blue
      {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}}   // top left, yellow
  };

  std::vector<unsigned int> indices = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  std::vector<std::unique_ptr<Mesh>> meshArray;
  meshArray.push_back(std::make_unique<Mesh>(vertices, indices));

  creature = std::make_unique<Creature>(std::move(meshArray));

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    creature->Draw(*shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Engine::shutdown() {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
  running = false;
}
