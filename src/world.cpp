#include "engine/world.hpp"
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
  // Destroy any tail that belongs to this creature
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
