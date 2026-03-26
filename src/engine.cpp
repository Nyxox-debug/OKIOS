#include "engine/engine.hpp"
#include "engine/camera.hpp"
#include "engine/input.hpp"
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

void JointSystem(World &world) {
  for (auto &[id, joint] : world.joints) {
    if (!world.transforms.count(id) || !world.transforms.count(joint.parentID))
      continue;
    auto &parentTrans = world.transforms.at(joint.parentID).transform;
    auto &childTrans = world.transforms.at(id).transform;
    childTrans.position = parentTrans.position + joint.localOffset;
  }
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
  world.init();

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

  // for (int i = 0; i < 10; i++) {
  //   int entity = world.createEntity();
  //   TransformComponent t;
  //   VelocityComponent v;
  //   MeshComponent m;
  //   m.mesh = std::make_shared<Mesh>(vertices, indices);
  //   t.transform.position = {(float)i, 0.0f, 0.0f};
  //   t.transform.scale = {1.0f, 1.0f, 1.0f};
  //   v.velocity = {0.0f, 0.0f, 0.0f};
  //
  //   world.addMeshComponent(entity, m);
  //   world.addTransformComponent(entity, t);
  //   world.addVelocityComponent(entity, v);
  // }

  // Segment 1 — torso (has velocity, moves around)
  int torso = world.createEntity();
  {
    TransformComponent t;
    VelocityComponent v;
    MeshComponent m;
    m.mesh = std::make_shared<Mesh>(vertices, indices);
    t.transform.position = {5.0f, 2.0f, 5.0f};
    t.transform.scale = {1.0f, 1.0f, 1.0f};
    v.velocity = {0.0f, 0.0f, 0.0f};
    world.addTransformComponent(torso, t);
    world.addVelocityComponent(torso, v);
    world.addMeshComponent(torso, m);
  }

  // Segment 2 — attached to right of torso
  int seg2 = world.createEntity();
  {
    TransformComponent t;
    MeshComponent m;
    m.mesh = std::make_shared<Mesh>(vertices, indices);
    t.transform.position = {0.0f, 0.0f, 0.0f};
    t.transform.scale = {1.0f, 1.0f, 1.0f};
    world.addTransformComponent(seg2, t);
    world.addMeshComponent(seg2, m);
    world.addJointComponent(seg2, JointComponent{torso, {1.5f, 0.0f, 0.0f}});
  }

  // Segment 3 — attached to right of seg2
  int seg3 = world.createEntity();
  {
    TransformComponent t;
    MeshComponent m;
    m.mesh = std::make_shared<Mesh>(vertices, indices);
    t.transform.position = {0.0f, 0.0f, 0.0f};
    t.transform.scale = {1.0f, 1.0f, 1.0f};
    world.addTransformComponent(seg3, t);
    world.addMeshComponent(seg3, m);
    world.addJointComponent(seg3, JointComponent{seg2, {1.5f, 0.0f, 0.0f}});
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
    shader->setMat4("model", world.terrain->modelMat);
    shader->setMat3("normalMatrix", world.terrain->normalMat);
    world.terrain->Draw(*shader);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (auto &[id, trans] : world.transforms) {
      glm::mat4 model = trans.transform.getModelMatrix();
      shader->setMat4("model", model);
      shader->setMat3("normalMatrix", trans.normalMatrix);

      if (world.meshes.count(id)) {
        world.meshes.at(id).mesh->Draw(*shader);
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
void CollisionSystem(World &world) {
  for (auto &[idA, transA] : world.transforms) {
    for (auto &[idB, transB] : world.transforms) {
      if (idA >= idB)
        continue;
      bool bothHaveVelocity =
          world.velocities.count(idA) && world.velocities.count(idB);
      glm::vec3 aMin =
          transA.transform.position + glm::vec3(-0.5f, -0.5f, -0.5f);
      glm::vec3 aMax = transA.transform.position + glm::vec3(0.5f, 0.5f, 0.5f);

      glm::vec3 bMin =
          transB.transform.position + glm::vec3(-0.5f, -0.5f, -0.5f);
      glm::vec3 bMax = transB.transform.position + glm::vec3(0.5f, 0.5f, 0.5f);

      bool overlaps = aMin.x < bMax.x && bMin.x < aMax.x && aMin.y < bMax.y &&
                      bMin.y < aMax.y && aMin.z < bMax.z && bMin.z < aMax.z;

      float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
      float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
      float overlapZ = std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z);

      if (overlaps) {

        if (overlapX < overlapY && overlapX < overlapZ) {
          float pushX = overlapX * 0.5f;
          if (transA.transform.position.x < transB.transform.position.x) {
            transA.transform.position.x -= pushX;
            transB.transform.position.x += pushX;
          } else {
            transA.transform.position.x += pushX;
            transB.transform.position.x -= pushX;
          }
          if (bothHaveVelocity) {
            auto &velA = world.velocities.at(idA);
            auto &velB = world.velocities.at(idB);
            std::swap(velA.velocity.x, velB.velocity.x);
          }
        } else if (overlapY < overlapZ) {
          float pushY = overlapY * 0.5f;
          if (transA.transform.position.y < transB.transform.position.y) {
            transA.transform.position.y -= pushY;
            transB.transform.position.y += pushY;
          } else {
            transA.transform.position.y += pushY;
            transB.transform.position.y -= pushY;
          }

          if (bothHaveVelocity) {
            auto &velA = world.velocities.at(idA);
            auto &velB = world.velocities.at(idB);
            std::swap(velA.velocity.y, velB.velocity.y);
          }
        } else {
          float pushZ = overlapZ * 0.5f;
          if (transA.transform.position.z < transB.transform.position.z) {
            transA.transform.position.z -= pushZ;
            transB.transform.position.z += pushZ;
          } else {
            transA.transform.position.z += pushZ;
            transB.transform.position.z -= pushZ;
          }
          if (bothHaveVelocity) {
            auto &velA = world.velocities.at(idA);
            auto &velB = world.velocities.at(idB);
            std::swap(velA.velocity.z, velB.velocity.z);
          }
        }
      }
    }
  }
};

void MovementSystem(World &world, float dt) {
  for (auto &[id, vel] : world.velocities) {
    if (world.transforms.count(id)) {
      auto &trans = world.transforms.at(id);
      vel.velocity.x += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;

      vel.velocity.z += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;

      // Gravity
      vel.velocity.y += -9.8f * dt;

      float maxSpeed = 2.0f;
      vel.velocity.x = glm::clamp(vel.velocity.x, -maxSpeed, maxSpeed);
      vel.velocity.z = glm::clamp(vel.velocity.z, -maxSpeed, maxSpeed);

      vel.velocity.x *= 0.95f;
      vel.velocity.z *= 0.95f;

      trans.transform.position.x += vel.velocity.x * dt;
      trans.transform.position.z += vel.velocity.z * dt;

      trans.transform.position.x =
          glm::clamp(trans.transform.position.x, world.terrain->bounds.x,
                     world.terrain->bounds.y);
      trans.transform.position.z =
          glm::clamp(trans.transform.position.z, world.terrain->bounds.z,
                     world.terrain->bounds.w);

      // trans.transform.position.y = world.terrain->terrainHeight(
      //     trans.transform.position.x, trans.transform.position.z);
      trans.transform.position.y += vel.velocity.y * dt;

      float groundY = world.terrain->terrainHeight(trans.transform.position.x,
                                                   trans.transform.position.z);
      if (trans.transform.position.y < groundY) {
        vel.velocity.y = 0;
        trans.transform.position.y = groundY;
      }

      if (vel.velocity.x != 0.0f || vel.velocity.z != 0.0f) {
        float targetYaw = atan2(vel.velocity.x, vel.velocity.z);
        trans.transform.rotation.y =
            glm::mix(trans.transform.rotation.y, targetYaw, 0.1f);
      }

      trans.normalMatrix = glm::mat3(glm::transpose(glm::inverse(
          trans.transform
              .getModelMatrix()))); // NOTE: I used normalMatrix for
                                    // transforms under non-uniform scale
    }
  }
}

void Engine::update(float dt) {
  MovementSystem(world, dt);
  JointSystem(world);
  CollisionSystem(world);
}

void Engine::shutdown() {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
  running = false;
}
