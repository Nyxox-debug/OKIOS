#include "engine/engine.hpp"
#include "engine/brain.hpp"
#include "engine/camera.hpp"
#include "engine/input.hpp"
#include "glm/fwd.hpp"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
// clang-format on

#include <glm/common.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <random>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

constexpr int SIM_STEPS_PER_FRAME = 60;
constexpr float FIXED_DT = 0.016f;
constexpr int MIN_POPULATION = 6;
constexpr int MAX_POPULATION = 40;

// Helpers

static std::mt19937 &getRng() {
  static std::mt19937 rng(std::random_device{}());
  return rng;
}

static float randRange(float lo, float hi) {
  return std::uniform_real_distribution<float>(lo, hi)(getRng());
}

static float randInt(int lo, int hi) { // inclusive
  return std::uniform_int_distribution<int>(lo, hi)(getRng());
}

static void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

Engine::Engine() = default;
Engine::~Engine() = default;

void Engine::processInput() {
  if (Input::IsKeyPressed(window, GLFW_KEY_W))
    camera->ProcessKeyboard(FORWARD, deltaTime);
  if (Input::IsKeyPressed(window, GLFW_KEY_S))
    camera->ProcessKeyboard(BACKWARD, deltaTime);
  if (Input::IsKeyPressed(window, GLFW_KEY_A))
    camera->ProcessKeyboard(LEFT, deltaTime);
  if (Input::IsKeyPressed(window, GLFW_KEY_D))
    camera->ProcessKeyboard(RIGHT, deltaTime);
  if (Input::IsKeyPressed(window, GLFW_KEY_ESCAPE))
    glfwSetWindowShouldClose(window, true);
}

bool Engine::init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simulation", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return false;
  }

  shader = std::make_unique<Shader>("../res/shaders/phong.vert",
                                    "../res/shaders/phong.frag");

  camera =
      std::make_unique<Camera>(glm::vec3(25.0f, 30.0f, 25.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f), -135.0f, -45.0f);
  camera->MovementSpeed = 20.0f;

  glfwSetWindowUserPointer(window, this);
  glfwSetCursorPosCallback(
      window, [](GLFWwindow *win, double xpos, double ypos) {
        static float lastX = SCR_WIDTH / 2.0f;
        static float lastY = SCR_HEIGHT / 2.0f;
        static bool firstMouse = true;
        if (firstMouse) {
          lastX = xpos;
          lastY = ypos;
          firstMouse = false;
        }
        float xoff = xpos - lastX;
        float yoff = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        auto *e = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(win));
        if (e && e->camera)
          e->camera->ProcessMouseMovement(xoff, yoff);
      });
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);
  world.init();
  running = true;
  return true;
}

static std::pair<std::vector<Vertex>, std::vector<unsigned int>> makeCube() {
  std::vector<Vertex> verts = {
      // Front (slightly brighter)
      {{0.5f, 0.5f, 0.5f}, {0.25f, 0.25f, 0.25f}, {0, 0, 1}},
      {{0.5f, -0.5f, 0.5f}, {0.25f, 0.25f, 0.25f}, {0, 0, 1}},
      {{-0.5f, -0.5f, 0.5f}, {0.25f, 0.25f, 0.25f}, {0, 0, 1}},
      {{-0.5f, 0.5f, 0.5f}, {0.25f, 0.25f, 0.25f}, {0, 0, 1}},

      // Back (darker)
      {{-0.5f, 0.5f, -0.5f}, {0.12f, 0.12f, 0.12f}, {0, 0, -1}},
      {{-0.5f, -0.5f, -0.5f}, {0.12f, 0.12f, 0.12f}, {0, 0, -1}},
      {{0.5f, -0.5f, -0.5f}, {0.12f, 0.12f, 0.12f}, {0, 0, -1}},
      {{0.5f, 0.5f, -0.5f}, {0.12f, 0.12f, 0.12f}, {0, 0, -1}},

      // Right
      {{0.5f, 0.5f, -0.5f}, {0.2f, 0.2f, 0.2f}, {1, 0, 0}},
      {{0.5f, -0.5f, -0.5f}, {0.2f, 0.2f, 0.2f}, {1, 0, 0}},
      {{0.5f, -0.5f, 0.5f}, {0.2f, 0.2f, 0.2f}, {1, 0, 0}},
      {{0.5f, 0.5f, 0.5f}, {0.2f, 0.2f, 0.2f}, {1, 0, 0}},

      // Left
      {{-0.5f, 0.5f, 0.5f}, {0.18f, 0.18f, 0.18f}, {-1, 0, 0}},
      {{-0.5f, -0.5f, 0.5f}, {0.18f, 0.18f, 0.18f}, {-1, 0, 0}},
      {{-0.5f, -0.5f, -0.5f}, {0.18f, 0.18f, 0.18f}, {-1, 0, 0}},
      {{-0.5f, 0.5f, -0.5f}, {0.18f, 0.18f, 0.18f}, {-1, 0, 0}},

      // Top (light catches here)
      {{-0.5f, 0.5f, -0.5f}, {0.3f, 0.3f, 0.3f}, {0, 1, 0}},
      {{0.5f, 0.5f, -0.5f}, {0.3f, 0.3f, 0.3f}, {0, 1, 0}},
      {{0.5f, 0.5f, 0.5f}, {0.3f, 0.3f, 0.3f}, {0, 1, 0}},
      {{-0.5f, 0.5f, 0.5f}, {0.3f, 0.3f, 0.3f}, {0, 1, 0}},

      // Bottom (darkest)
      {{-0.5f, -0.5f, 0.5f}, {0.1f, 0.1f, 0.1f}, {0, -1, 0}},
      {{0.5f, -0.5f, 0.5f}, {0.1f, 0.1f, 0.1f}, {0, -1, 0}},
      {{0.5f, -0.5f, -0.5f}, {0.1f, 0.1f, 0.1f}, {0, -1, 0}},
      {{-0.5f, -0.5f, -0.5f}, {0.1f, 0.1f, 0.1f}, {0, -1, 0}},
  };

  std::vector<unsigned int> idx;
  for (unsigned int f = 0; f < 6; f++) {
    unsigned int b = f * 4;
    idx.insert(idx.end(), {b, b + 1, b + 2, b, b + 2, b + 3});
  }
  return {verts, idx};
}

static void spawnCreature(World &world, const std::vector<Vertex> &verts,
                          const std::vector<unsigned int> &idx, Brain brain) {
  float x = randRange(-50.0f, 50.0f);
  float z = randRange(-50.0f, 50.0f);
  float y = world.terrain->terrainHeight(x, z);

  int id = world.createEntity();
  TransformComponent t;
  t.transform.position = {x, y, z};
  t.transform.scale = {1, 1, 1};
  VelocityComponent v;
  v.velocity = {0, 0, 0};
  MeshComponent m;
  m.mesh = std::make_shared<Mesh>(verts, idx);

  world.addTransformComponent(id, t);
  world.addVelocityComponent(id, v);
  world.addMeshComponent(id, m);
  world.addLifeComponent(id, LifeComponent{100.0f, 100.0f, 0.0f, 100.0f});
  world.addMotorComponent(id, MotorComponent{glm::vec3(0.0f), 5.0f});

  BrainComponent b;
  b.brain = brain;
  world.addBrainComponent(id, b);

  // Tail
  std::vector<Vertex> tailVerts = verts;
  for (auto &tv : tailVerts)
    tv.Color = {1.0f, 0.0f, 0.0f};

  int tailID = world.createEntity();
  TransformComponent tt;
  tt.transform.position = {x, y, z}; // JointSystem will correct this each frame
  tt.transform.scale = {0.35f, 0.35f, 0.35f};
  MeshComponent tm;
  tm.mesh = std::make_shared<Mesh>(tailVerts, idx);

  world.addTransformComponent(tailID, tt);
  world.addMeshComponent(tailID, tm);
  world.addJointComponent(tailID,
                          JointComponent{id, glm::vec3(0.0f, 0.3f, -1.2f)});
  world.addTailComponent(tailID, id);
}

static void spawnFood(World &world, const std::vector<Vertex> &verts,
                      const std::vector<unsigned int> &idx) {
  float x = randRange(-35.0f, 35.0f);
  float z = randRange(-35.0f, 35.0f);
  float y = world.terrain->terrainHeight(x, z);

  int id = world.createEntity();
  TransformComponent t;
  t.transform.position = {x, y + 0.3f, z};
  t.transform.scale = {0.3f, 0.3f, 0.3f};
  MeshComponent m;
  m.mesh = std::make_shared<Mesh>(verts, idx);
  world.addTransformComponent(id, t);
  world.addMeshComponent(id, m);
  world.addFoodComponent(id, FoodComponent{1.0f});
}

// Systems

static void LightSystem(World &world, Shader &shader) {
  for (auto &[id, light] : world.lightSources) {
    shader.setVec3("lightDir", light.direction);
    shader.setVec3("lightColor", light.color);
  }
}

static void JointSystem(World &world) {
  for (auto &[id, joint] : world.joints) {
    if (!world.transforms.count(id) || !world.transforms.count(joint.parentID))
      continue;
    auto &parentTrans = world.transforms.at(joint.parentID).transform;
    auto &childTrans = world.transforms.at(id).transform;
    glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), parentTrans.rotation.y,
                                   glm::vec3(0, 1, 0));
    childTrans.position =
        parentTrans.position +
        glm::vec3(rotMat * glm::vec4(joint.localOffset, 0.0f));
  }
}

static void BrainSystem(World &world, float dt) {
  for (auto &[id, sentient] : world.sentients) {
    if (!world.lives.count(id) || !world.motors.count(id) ||
        !world.transforms.count(id) || !world.velocities.count(id))
      continue;

    // Nearest food
    auto &motor = world.motors.at(id);
    glm::vec3 agentPos = world.transforms.at(id).transform.position;
    glm::vec3 nearestFood = motor.target;
    float nearestDist = FLT_MAX;
    for (auto &[foodID, food] : world.foods) {
      glm::vec3 foodPos = world.transforms.at(foodID).transform.position;
      float dist = glm::length(agentPos - foodPos);
      if (dist < nearestDist) {
        nearestDist = dist;
        nearestFood = foodPos;
      }
    }
    motor.target = nearestFood;

    glm::vec3 diff = nearestFood - agentPos;
    glm::vec2 dir2D = glm::normalize(glm::vec2(diff.x, diff.z));
    float distToFood = glm::length(diff);

    auto &life = world.lives.at(id);
    input in = {life.health / life.maxHealth, life.hunger / life.maxHunger,
                dir2D, glm::clamp(distToFood / 50.0f, 0.0f, 1.0f)};
    output out = sentient.brain.forward(in);

    glm::vec3 force = glm::vec3(out.direction.x, 0.0f, out.direction.y);
    world.velocities.at(id).velocity += force * motor.strength * dt;
  }
}

static void MovementSystem(World &world, float dt) {
  for (auto &[id, vel] : world.velocities) {
    if (!world.transforms.count(id))
      continue;
    auto &trans = world.transforms.at(id);

    vel.velocity.y += -9.8f * dt;

    const float maxSpeed = 2.0f;
    vel.velocity.x = glm::clamp(vel.velocity.x, -maxSpeed, maxSpeed);
    vel.velocity.z = glm::clamp(vel.velocity.z, -maxSpeed, maxSpeed);
    vel.velocity.x *= 0.95f;
    vel.velocity.z *= 0.95f;

    trans.transform.position += vel.velocity * dt;

    if (trans.transform.position.x <= world.terrain->bounds.x ||
        trans.transform.position.x >= world.terrain->bounds.y) {
      trans.transform.position.x =
          glm::clamp(trans.transform.position.x, world.terrain->bounds.x,
                     world.terrain->bounds.y);
      vel.velocity.x = 0.0f;
    }

    if (trans.transform.position.z <= world.terrain->bounds.z ||
        trans.transform.position.z >= world.terrain->bounds.w) {
      trans.transform.position.z =
          glm::clamp(trans.transform.position.z, world.terrain->bounds.z,
                     world.terrain->bounds.w);
      vel.velocity.z = 0.0f;
    }

    float groundY = world.terrain->terrainHeight(trans.transform.position.x,
                                                 trans.transform.position.z);
    if (trans.transform.position.y < groundY) {
      vel.velocity.y = 0.0f;
      trans.transform.position.y = groundY;
    }

    if (vel.velocity.x != 0.0f || vel.velocity.z != 0.0f) {
      float targetYaw = std::atan2(vel.velocity.x, vel.velocity.z);
      trans.transform.rotation.y =
          glm::mix(trans.transform.rotation.y, targetYaw, 0.1f);
    }

    trans.normalMatrix = glm::mat3(
        glm::transpose(glm::inverse(trans.transform.getModelMatrix())));
  }
}

static void FoodSystem(World &world, const std::vector<Vertex> &foodVerts,
                       const std::vector<unsigned int> &foodIdx) {
  for (auto &[foodID, food] : world.foods) {
    if (!world.transforms.count(foodID))
      continue;
    glm::vec3 foodPos = world.transforms.at(foodID).transform.position;

    for (auto &[agentID, life] : world.lives) {
      if (!world.transforms.count(agentID))
        continue;
      if (world.tails.count(agentID))
        continue;
      float dist = glm::length(world.transforms.at(agentID).transform.position -
                               foodPos);
      if (dist < 1.5f) {
        float x = randRange(-50.0f, 50.0f);
        float z = randRange(-50.0f, 50.0f);
        float y = world.terrain->terrainHeight(x, z);
        world.transforms.at(foodID).transform.position = {x, y + 0.3f, z};
        life.hunger = 0.0f;
        life.mealAmount += 1;
        break;
      }
    }
  }
}

static void LifeSystem(World &world, float dt) {
  std::vector<int> dead;
  for (auto &[id, life] : world.lives) {
    life.hunger = glm::clamp(life.hunger + 0.003f * dt, 0.0f, life.maxHunger);
    life.health -= (life.hunger / life.maxHunger) * 0.8f * dt;
    life.reward =
        (life.health / life.maxHealth) - (life.hunger / life.maxHunger);
    life.cumulativeReward += life.reward * dt;
    if (life.health <= 0.0f)
      dead.push_back(id);
  }

  for (int id : dead)
    world.destroyEntity(id);
}

static void ReproductionSystem(World &world, const std::vector<Vertex> &verts,
                               const std::vector<unsigned int> &idx) {
  if ((int)world.sentients.size() >= MAX_POPULATION)
    return;

  std::vector<int> toReproduce;
  for (auto &[id, life] : world.lives)
    if (life.mealAmount >= 4 && world.sentients.count(id))
      toReproduce.push_back(id);

  for (int id : toReproduce) {
    if ((int)world.sentients.size() >= MAX_POPULATION)
      break;
    Brain childBrain = world.sentients.at(id).brain.mutate(0.1f, getRng());
    spawnCreature(world, verts, idx, childBrain);
    world.lives.at(id).mealAmount = 0;
  }
}

static void CollisionSystem(World &world) {
  for (auto &[idA, transA] : world.transforms) {
    // Skip tail entities — they have no velocity and shouldn't push things
    if (world.tails.count(idA))
      continue;

    for (auto &[idB, transB] : world.transforms) {
      if (idA >= idB)
        continue;
      if (world.tails.count(idB))
        continue;

      glm::vec3 aMin = transA.transform.position - glm::vec3(0.5f);
      glm::vec3 aMax = transA.transform.position + glm::vec3(0.5f);
      glm::vec3 bMin = transB.transform.position - glm::vec3(0.5f);
      glm::vec3 bMax = transB.transform.position + glm::vec3(0.5f);

      if (aMin.x >= bMax.x || bMin.x >= aMax.x || aMin.y >= bMax.y ||
          bMin.y >= aMax.y || aMin.z >= bMax.z || bMin.z >= aMax.z)
        continue;

      float ox = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
      float oy = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
      float oz = std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z);

      bool bothDynamic =
          world.velocities.count(idA) && world.velocities.count(idB);
      auto &pA = transA.transform.position;
      auto &pB = transB.transform.position;

      if (ox < oy && ox < oz) {
        float half = ox * 0.5f;
        if (pA.x < pB.x) {
          pA.x -= half;
          pB.x += half;
        } else {
          pA.x += half;
          pB.x -= half;
        }
        if (bothDynamic)
          std::swap(world.velocities.at(idA).velocity.x,
                    world.velocities.at(idB).velocity.x);
      } else if (oy < oz) {
        float half = oy * 0.5f;
        if (pA.y < pB.y) {
          pA.y -= half;
          pB.y += half;
        } else {
          pA.y += half;
          pB.y -= half;
        }
        if (bothDynamic)
          std::swap(world.velocities.at(idA).velocity.y,
                    world.velocities.at(idB).velocity.y);
      } else {
        float half = oz * 0.5f;
        if (pA.z < pB.z) {
          pA.z -= half;
          pB.z += half;
        } else {
          pA.z += half;
          pB.z -= half;
        }
        if (bothDynamic)
          std::swap(world.velocities.at(idA).velocity.z,
                    world.velocities.at(idB).velocity.z);
      }
    }
  }
}

void Engine::run() {
  if (!running)
    return;

  auto [verts, idx] = makeCube();

  std::vector<Vertex> foodVerts = verts;
  for (auto &v : foodVerts)
    v.Color = {1.0f, 0.8f, 0.0f};

  world.creatureVertices = verts;
  world.creatureIndices = idx;

  // Light
  int light = world.createEntity();
  world.addLightComponent(light, LightComponent{glm::vec3(0.0f, -1.0f, 0.2f),
                                                glm::vec3(1.0f, 1.0f, 1.0f)});

  // Food
  for (int i = 0; i < 30; i++)
    spawnFood(world, foodVerts, idx);

  // Creatures
  for (int i = 0; i < 10; i++) {
    Brain b;
    b.init(getRng());
    spawnCreature(world, verts, idx, b);
  }

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput();

    for (int i = 0; i < SIM_STEPS_PER_FRAME; i++)
      update(FIXED_DT);

    // Population floor rescue
    int creatureCount = (int)world.sentients.size();
    if (creatureCount < MIN_POPULATION) {
      Brain seedBrain;
      if (!world.sentients.empty())
        seedBrain =
            world.sentients.begin()->second.brain.mutate(0.05f, getRng());
      else
        seedBrain.init(getRng());

      int needed = MIN_POPULATION - creatureCount;
      for (int i = 0; i < needed; i++)
        spawnCreature(world, verts, idx, seedBrain.mutate(0.1f, getRng()));
    }

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera->Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("viewPos", camera->Position);

    LightSystem(world, *shader);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shader->setMat4("model", world.terrain->modelMat);
    shader->setMat3("normalMatrix", world.terrain->normalMat);
    world.terrain->Draw(*shader);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (auto &[id, trans] : world.transforms) {
      shader->setMat4("model", trans.transform.getModelMatrix());
      shader->setMat3("normalMatrix", trans.normalMatrix);
      if (world.meshes.count(id))
        world.meshes.at(id).mesh->Draw(*shader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Engine::update(float dt) {
  BrainSystem(world, dt);
  MovementSystem(world, dt);
  FoodSystem(world, world.creatureVertices, world.creatureIndices);
  LifeSystem(world, dt);
  ReproductionSystem(world, world.creatureVertices, world.creatureIndices);
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
