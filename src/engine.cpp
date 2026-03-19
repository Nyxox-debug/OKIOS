#include "engine/engine.hpp"
#include "engine/camera.h"
#include "engine/creature/creature.hpp"
#include <iostream>
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void Engine::processInput() {

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->ProcessKeyboard(RIGHT, deltaTime);

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

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

  // Mouse Movement
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos,
                                      double ypos) {
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed
    lastX = xpos;
    lastY = ypos;

    auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
    engine->camera->ProcessMouseMovement(xoffset, yoffset);
  });
  glfwSetWindowUserPointer(window, this);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  shader = std::make_unique<Shader>("../res/shaders/def.vert",
                                    "../res/shaders/def.frag");
  running = true;

  camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), // Position
                                    glm::vec3(0.0f, 1.0f, 0.0f), // Up vector
                                    -90.0f, 0.0f                 // Yaw, Pitch
  );

  return true;
};

void Engine::run() {
  if (!running)
    return;

  std::vector<Vertex> vertices = {
      // Front face
      {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},   // 0 top right front, red
      {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},  // 1 bottom right front, green
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // 2 bottom left front, blue
      {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},  // 3 top left front, yellow
      // Back face
      {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},   // 4 top right back, magenta
      {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},  // 5 bottom right back, cyan
      {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 6 bottom left back, white
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}   // 7 top left back, black
  };

  std::vector<unsigned int> indices = {// Front face
                                       0, 1, 3, 1, 2, 3,
                                       // Right face
                                       0, 4, 1, 1, 4, 5,
                                       // Back face
                                       4, 7, 5, 5, 7, 6,
                                       // Left face
                                       3, 2, 7, 2, 6, 7,
                                       // Top face
                                       3, 7, 0, 0, 7, 4,
                                       // Bottom face
                                       1, 5, 2, 2, 5, 6};

  std::vector<std::unique_ptr<Mesh>> meshArray;
  meshArray.push_back(std::make_unique<Mesh>(vertices, indices));

  creature = std::make_unique<Creature>(std::move(meshArray));

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput();

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera->Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    float timeValue = glfwGetTime();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, timeValue, glm::vec3(0.5f, 1.0f, 0.0f));

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("model", model);

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
