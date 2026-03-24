#include "engine/world.hpp"
#include <optional>

int World::createEntity() {
  return nextID++;
}

void World::addVelocityComponent(int entityID, VelocityComponent vel) {
  velocities[entityID] = vel;
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
