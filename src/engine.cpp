#include "engine/engine.hpp"
#include "engine/camera.hpp"
#include "engine/input.hpp"
#include "engine/terrain.hpp"
#include "glm/fwd.hpp"
#include <iostream>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
// clang-format on

#include <ctime>
#include <glm/common.hpp>
#include <glm/gtc/matrix_inverse.hpp>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void Engine::processInput() {
  if (Input::IsKeyPressed(window, GLFW_KEY_W)) {
    camera->ProcessKeyboard(FORWARD, deltaTime);
  };
  if (Input::IsKeyPressed(window, GLFW_KEY_S)) {
    camera->ProcessKeyboard(BACKWARD, deltaTime);
  };
  if (Input::IsKeyPressed(window, GLFW_KEY_A)) {
    camera->ProcessKeyboard(LEFT, deltaTime);
  };
  if (Input::IsKeyPressed(window, GLFW_KEY_D)) {
    camera->ProcessKeyboard(RIGHT, deltaTime);
  };

  if (Input::IsKeyPressed(window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(window, true);
  };
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

  shader = std::make_unique<Shader>("../res/shaders/phong.vert",
                                    "../res/shaders/phong.frag");

  // camera =
  //     std::make_unique<Camera>(glm::vec3(4.5f, 20.0f, 18.0f),
  //                              glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -55.0f);

  // camera->MovementSpeed = 15.0f;

  camera =
      std::make_unique<Camera>(glm::vec3(25.0f, 30.0f, 25.0f), // high and back
                               glm::vec3(0.0f, 1.0f, 0.0f),    // up vector
                               -135.0f, // yaw: pointing diagonally
                               -45.0f   // pitch: looking down at 45°
      );

  camera->MovementSpeed = 20.0f; // faster movement for overview

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

  srand(time(NULL));

  running = true;
  return true;
}

void LightSystem(World &world, Shader &shader) {
  for (auto &[id, light] : world.lightSources) {
    shader.setVec3("lightDir", light.direction);
    shader.setVec3("lightColor", light.color);
  }
}

void Engine::run() {
  if (!running)
    return;

  terrain = std::make_unique<Terrain>(50, 1.0f);

  std::vector<Vertex> vertices = {
      // Front (+Z)
      {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {0, 0, 1}},
      {{0.5f, -0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}},
      {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 0, 1}},
      {{-0.5f, 0.5f, 0.5f}, {1, 1, 0}, {0, 0, 1}},
      // Back (-Z)
      {{-0.5f, 0.5f, -0.5f}, {1, 0, 1}, {0, 0, -1}},
      {{-0.5f, -0.5f, -0.5f}, {0, 1, 1}, {0, 0, -1}},
      {{0.5f, -0.5f, -0.5f}, {1, 1, 1}, {0, 0, -1}},
      {{0.5f, 0.5f, -0.5f}, {0, 0, 0}, {0, 0, -1}},
      // Right (+X)
      {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {1, 0, 0}},
      {{0.5f, -0.5f, -0.5f}, {0, 1, 0}, {1, 0, 0}},
      {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 0, 0}},
      {{0.5f, 0.5f, 0.5f}, {1, 1, 0}, {1, 0, 0}},
      // Left (-X)
      {{-0.5f, 0.5f, 0.5f}, {1, 0, 1}, {-1, 0, 0}},
      {{-0.5f, -0.5f, 0.5f}, {0, 1, 1}, {-1, 0, 0}},
      {{-0.5f, -0.5f, -0.5f}, {1, 1, 1}, {-1, 0, 0}},
      {{-0.5f, 0.5f, -0.5f}, {0, 0, 0}, {-1, 0, 0}},
      // Top (+Y)
      {{-0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 1, 0}},
      {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1, 0}},
      {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 1, 0}},
      {{-0.5f, 0.5f, 0.5f}, {1, 1, 0}, {0, 1, 0}},
      // Bottom (-Y)
      {{-0.5f, -0.5f, 0.5f}, {1, 0, 1}, {0, -1, 0}},
      {{0.5f, -0.5f, 0.5f}, {0, 1, 1}, {0, -1, 0}},
      {{0.5f, -0.5f, -0.5f}, {1, 1, 1}, {0, -1, 0}},
      {{-0.5f, -0.5f, -0.5f}, {0, 0, 0}, {0, -1, 0}},
  };

  std::vector<unsigned int> indices;
  for (unsigned int f = 0; f < 6; f++) {
    unsigned int b = f * 4;
    indices.insert(indices.end(), {b, b + 1, b + 2, b, b + 2, b + 3});
  }

  int light = world.createEntity();
  world.addLightComponent(light, LightComponent{
                                     glm::vec3(0.0f, -1.0f, 0.2f), // direction
                                     glm::vec3(1.0f, 1.0f, 1.0f)   // color
                                 });

  for (int i = 0; i < 10; i++) {
    int entity = world.createEntity();
    TransformComponent t;
    VelocityComponent v;
    MeshComponent m;
    m.mesh = std::make_shared<Mesh>(vertices, indices);
    t.transform.position = {(float)i, 0.0f, 0.0f};
    t.transform.scale = {1.0f, 1.0f, 1.0f};
    v.velocity = {0.0f, 0.0f, 0.0f};

    world.addMeshComponent(entity, m);
    world.addTransformComponent(entity, t);
    world.addVelocityComponent(entity, v);
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
    // // shader->setVec3("lightDir", glm::vec3(0.5f, -1.0f, 0.5f));
    // shader->setVec3("lightDir", glm::vec3(0.0f, -1.0f, 0.2f));
    // shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    LightSystem(world, *shader);

    shader->setVec3("viewPos", camera->Position);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel =
        glm::translate(terrainModel, glm::vec3(-25.0f, -0.5f, -25.0f));
    shader->setMat4("model", terrainModel);
    shader->setMat3("normalMatrix",
                    glm::mat3(glm::transpose(glm::inverse(terrainModel))));
    terrain->Draw(*shader);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (auto &[id, trans] : world.transforms) {
      glm::mat4 model = trans.transform.getModelMatrix();
      shader->setMat4("model", model);
      shader->setMat3("normalMatrix",
                      glm::mat3(glm::transpose(glm::inverse(model))));
      if (world.meshes.count(id)) {
        world.meshes.at(id).mesh->Draw(*shader);
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void MovementSystem(World &world, float dt) {
  for (auto &[id, vel] : world.velocities) {
    if (world.transforms.count(id)) {
      auto &trans = world.transforms.at(id);
      vel.velocity.x += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;

      vel.velocity.z += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;

      float maxSpeed = 2.0f;
      vel.velocity.x = glm::clamp(vel.velocity.x, -maxSpeed, maxSpeed);
      vel.velocity.z = glm::clamp(vel.velocity.z, -maxSpeed, maxSpeed);

      vel.velocity.x *= 0.95f;
      vel.velocity.z *= 0.95f;

      trans.transform.position.x += vel.velocity.x * dt;
      trans.transform.position.z += vel.velocity.z * dt;

      if (vel.velocity.x != 0.0f || vel.velocity.z != 0.0f) {
        float targetYaw = atan2(vel.velocity.x, vel.velocity.z);
        trans.transform.rotation.y =
            glm::mix(trans.transform.rotation.y, targetYaw, 0.1f);
      }
    }
  }
}

void Engine::update(float dt) { MovementSystem(world, dt); }

void Engine::shutdown() {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
  running = false;
}
