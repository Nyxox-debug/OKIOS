#include "engine/world.hpp"
#include <optional>

void World::init() {
  terrain = std::make_unique<Terrain>(50, 1.0f, glm::vec2{-25.0f, -25.0f});
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

void World::destroyEntity(int entityID) {
  velocities.erase(entityID);
  transforms.erase(entityID);
  meshes.erase(entityID);
  lightSources.erase(entityID);
}

std::optional<TransformComponent> World::getTransformComponent(int entityID) {
  if (transforms.count(entityID))
    return transforms.at(entityID);
  return std::nullopt;
};

std::optional<VelocityComponent> World::getVelocityComponent(int entityID) {
  if (velocities.count(entityID))
    return velocities.at(entityID);
  return std::nullopt;
};
