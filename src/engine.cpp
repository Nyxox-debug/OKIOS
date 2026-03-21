#include "engine/engine.hpp"
#include "engine/camera.hpp"
#include "engine/creature/creature.hpp"
#include "engine/terrain.hpp"
#include <iostream>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
// clang-format on

#include <ctime>
#include <glm/common.hpp>

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

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simulation", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD\n";
    return false;
  }

  shader = std::make_unique<Shader>("../res/shaders/def.vert",
                                    "../res/shaders/def.frag");

  // camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f),
  //                                   glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,
  //                                   0.0f);

  camera = std::make_unique<Camera>(
      glm::vec3(
          4.5f, 20.0f,
          18.0f), // centered on X (0–9 midpoint), high up, pulled back on Z
      glm::vec3(0.0f, 1.0f, 0.0f),
      -90.0f, // yaw: facing straight along -Z
      -55.0f  // pitch: steeper downward angle to see the ground plane clearly
  );

  camera->MovementSpeed = 15.0f;

  glfwSetWindowUserPointer(window, this);

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
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine || !engine->camera)
      return;

    engine->camera->ProcessMouseMovement(xoffset, yoffset);
  });

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);

  // Seed randomness once
  srand(time(NULL));

  running = true;
  return true;
}

void Engine::run() {
  if (!running)
    return;

  terrain = std::make_unique<Terrain>(50, 1.0f);

  std::vector<Vertex> vertices = {{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                                  {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                  {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                  {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
                                  {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
                                  {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
                                  {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                                  {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}};

  std::vector<unsigned int> indices = {0, 1, 3, 1, 2, 3, 0, 4, 1, 1, 4, 5,
                                       4, 7, 5, 5, 7, 6, 3, 2, 7, 2, 6, 7,
                                       3, 7, 0, 0, 7, 4, 1, 5, 2, 2, 5, 6};

  creatures.clear();

  for (int i = 0; i < 10; i++) {
    std::vector<std::unique_ptr<Mesh>> meshArray;
    meshArray.push_back(std::make_unique<Mesh>(vertices, indices));

    auto c = std::make_unique<Creature>(std::move(meshArray));

    c->transform.position = {(float)i, 0.0f, 0.0f};
    c->velocity = {0.0f, 0.0f, 0.0f};
    c->transform.scale = {1.0f, 1.0f, 1.0f};

    creatures.push_back(std::move(c));
  }

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput();
    update(deltaTime);

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera->Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel =
        glm::translate(terrainModel, glm::vec3(-25.0f, -0.5f, -25.0f));
    shader->setMat4("model", terrainModel);
    terrain->Draw(*shader);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto &c : creatures) {

      shader->setMat4("model", c->transform.getModelMatrix());
      c->Draw(*shader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Engine::update(float dt) {
  for (auto &c : creatures) {
    // rand movement
    c->velocity.x += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
    c->velocity.z += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;

    float maxSpeed = 2.0f;

    c->velocity.x = glm::clamp(c->velocity.x, -maxSpeed, maxSpeed);
    c->velocity.z = glm::clamp(c->velocity.z, -maxSpeed, maxSpeed);

    // friction (prevents infinite sliding)
    c->velocity.x *= 0.95f;
    c->velocity.z *= 0.95f;

    c->transform.position.x += c->velocity.x * dt;
    c->transform.position.z += c->velocity.z * dt;

    if (c->velocity.x != 0.0f || c->velocity.z != 0.0f) {
      float targetYaw = atan2(c->velocity.x, c->velocity.z);
      c->transform.rotation.y = glm::mix(c->transform.rotation.y, targetYaw,
                                         0.1f); // 0.1f = smoothing factor
    }
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
