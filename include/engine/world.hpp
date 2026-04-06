#pragma once
#include "brain.hpp"
#include "glm/fwd.hpp"
#include "mesh.hpp"
#include "engine/stb_perlin.h"
#include "terrain.hpp"
#include "transform.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct VelocityComponent {
  glm::vec3 velocity;
};

struct BrainComponent {
  Brain brain;
  std::vector<experience> History;
  int frameCount = 0;
};

struct TransformComponent {
  Transform transform;
  glm::mat3 normalMatrix;
};

struct MeshComponent {
  std::shared_ptr<Mesh> mesh;
};

struct LightComponent {
  glm::vec3 direction;
  glm::vec3 color;
};

struct JointComponent {
  int parentID;
  glm::vec3 localOffset;
};

struct MotorComponent {
  glm::vec3 target;
  float strength;
};

struct FoodComponent {
  float nutritionValue;
};

struct LifeComponent {
  float health, maxHealth, hunger, maxHunger;
  float reward;
  int mealAmount;
  float cumulativeReward;
  int biomeID; // 0 = valley, 1 = midland, 2 = highland
};

// Per-biome stats, updated each frame by PopulationStatsSystem.
struct BiomeStats {
  int   population    = 0;
  float avgReward     = 0.0f;
  float avgMeals      = 0.0f;
  int   biomeID       = 0;
};

class World {
private:
  int nextID = 0;

public:
  std::unordered_map<int, VelocityComponent>  velocities;
  std::unordered_map<int, TransformComponent> transforms;
  std::unordered_map<int, MeshComponent>      meshes;
  std::unordered_map<int, LightComponent>     lightSources;
  std::unordered_map<int, JointComponent>     joints;
  std::unordered_map<int, MotorComponent>     motors;
  std::unordered_map<int, FoodComponent>      foods;
  std::unordered_map<int, LifeComponent>      lives;
  std::unordered_map<int, BrainComponent>     sentients;
  std::unordered_map<int, int>                tails;

  std::vector<Vertex>       creatureVertices;
  std::vector<unsigned int> creatureIndices;

  std::unique_ptr<Terrain> terrain;

  float foodTime = 0.0f;

  // Phase 14: per-biome stats, readable by engine for HUD/debug.
  BiomeStats biomeStats[3];

  float foodDensityAt(float x, float z) const {
    // Biome modulates base food density:
    // valleys get a bonus, highlands get a penalty.
    float base = stb_perlin_noise3(x * 0.04f, foodTime * 0.08f, z * 0.04f,
                                   0, 0, 0);
    float biome = terrain->biomeAt(x, z); // [-1, 1]
    // biome < 0 → valley → more food; biome > 0 → highland → less food
    float modulated = base - biome * 0.4f;
    return glm::clamp(modulated, -1.0f, 1.0f);
  }

  // Phase 15: save all brains to a binary file.
  // Format: [int count] then count * 264-byte Brain records.
  void savePopulation(const std::string &path) const;

  // Phase 15: load brains from file, spawn creatures with them.
  // Existing population is NOT cleared — call clearCreatures() first if needed.
  void loadPopulation(const std::string &path);

  // Destroy all creature entities (sentients + their tails).
  void clearCreatures();

  void addVelocityComponent(int entityID, VelocityComponent vel);
  void addTransformComponent(int entityID, TransformComponent trans);
  void addMeshComponent(int entityID, MeshComponent mesh);
  void addLightComponent(int entityID, LightComponent light);
  void addJointComponent(int entityID, JointComponent joint);
  void addMotorComponent(int entityID, MotorComponent motor);
  void addFoodComponent(int entityID, FoodComponent food);
  void addLifeComponent(int entityID, LifeComponent life);
  void addBrainComponent(int entityID, BrainComponent brain);
  void addTailComponent(int tailID, int parentID);
  void destroyEntity(int entityID);
  int  createEntity();

  std::optional<VelocityComponent>  getVelocityComponent(int entityID);
  std::optional<TransformComponent> getTransformComponent(int entityID);

  void init();
};
