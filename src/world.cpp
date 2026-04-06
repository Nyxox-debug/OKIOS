#include "engine/world.hpp"
#include <fstream>
#include <stdexcept>
#include <optional>

void World::init() {
  terrain = std::make_unique<Terrain>(100, 1.0f, glm::vec2{-50.0f, -50.0f});
}

int World::createEntity() { return nextID++; }

void World::addVelocityComponent(int entityID, VelocityComponent vel) {
  velocities[entityID] = vel;
}
void World::addJointComponent(int entityID, JointComponent joint) {
  joints[entityID] = joint;
}
void World::addMeshComponent(int entityID, MeshComponent mesh) {
  meshes[entityID] = mesh;
}
void World::addTransformComponent(int entityID, TransformComponent trans) {
  transforms[entityID] = trans;
}
void World::addLightComponent(int entityID, LightComponent light) {
  lightSources[entityID] = light;
}
void World::addMotorComponent(int entityID, MotorComponent motor) {
  motors[entityID] = motor;
}
void World::addFoodComponent(int entityID, FoodComponent food) {
  foods[entityID] = food;
}
void World::addLifeComponent(int entityID, LifeComponent life) {
  lives[entityID] = life;
}
void World::addBrainComponent(int entityID, BrainComponent brain) {
  sentients[entityID] = brain;
}
void World::addTailComponent(int tailID, int parentID) {
  tails[tailID] = parentID;
}

void World::destroyEntity(int entityID) {
  std::vector<int> tailsToDestroy;
  for (auto &[tailID, parentID] : tails)
    if (parentID == entityID)
      tailsToDestroy.push_back(tailID);
  for (int tailID : tailsToDestroy) {
    tails.erase(tailID);
    joints.erase(tailID);
    transforms.erase(tailID);
    meshes.erase(tailID);
  }
  velocities.erase(entityID);
  transforms.erase(entityID);
  meshes.erase(entityID);
  lightSources.erase(entityID);
  sentients.erase(entityID);
  joints.erase(entityID);
  lives.erase(entityID);
  motors.erase(entityID);
  tails.erase(entityID);
}

std::optional<TransformComponent> World::getTransformComponent(int entityID) {
  if (transforms.count(entityID))
    return transforms.at(entityID);
  return std::nullopt;
}

std::optional<VelocityComponent> World::getVelocityComponent(int entityID) {
  if (velocities.count(entityID))
    return velocities.at(entityID);
  return std::nullopt;
}

void World::clearCreatures() {
  std::vector<int> toDestroy;
  for (auto &[id, _] : sentients)
    toDestroy.push_back(id);
  for (int id : toDestroy)
    destroyEntity(id);
}

void World::savePopulation(const std::string &path) const {
  std::ofstream f(path, std::ios::binary);
  if (!f)
    throw std::runtime_error("World::savePopulation — cannot open: " + path);

  int count = (int)sentients.size();
  f.write(reinterpret_cast<const char *>(&count), sizeof(int));

  for (auto &[id, bc] : sentients) {
    f.write(reinterpret_cast<const char *>(bc.brain.weightsInputHidden.data()),
            bc.brain.weightsInputHidden.size() * sizeof(float));
    f.write(reinterpret_cast<const char *>(bc.brain.weightsHiddenOutput.data()),
            bc.brain.weightsHiddenOutput.size() * sizeof(float));
    f.write(reinterpret_cast<const char *>(bc.brain.biasHidden.data()),
            bc.brain.biasHidden.size() * sizeof(float));
    f.write(reinterpret_cast<const char *>(bc.brain.biasOutput.data()),
            bc.brain.biasOutput.size() * sizeof(float));
  }
}

void World::loadPopulation(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f)
    throw std::runtime_error("World::loadPopulation — cannot open: " + path);

  int count = 0;
  f.read(reinterpret_cast<char *>(&count), sizeof(int));

  for (int i = 0; i < count; i++) {
    Brain b;
    f.read(reinterpret_cast<char *>(b.weightsInputHidden.data()),
           b.weightsInputHidden.size() * sizeof(float));
    f.read(reinterpret_cast<char *>(b.weightsHiddenOutput.data()),
           b.weightsHiddenOutput.size() * sizeof(float));
    f.read(reinterpret_cast<char *>(b.biasHidden.data()),
           b.biasHidden.size() * sizeof(float));
    f.read(reinterpret_cast<char *>(b.biasOutput.data()),
           b.biasOutput.size() * sizeof(float));

    (void)b;
  }
}
