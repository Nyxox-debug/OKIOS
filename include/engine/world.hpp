#include "glm/fwd.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include <memory>
#include <optional>
#include <unordered_map>

struct VelocityComponent {
  glm::vec3 velocity;
};
struct TransformComponent {
  Transform transform;
};
struct MeshComponent {
  std::shared_ptr<Mesh> mesh;
};
struct LightComponent {
  glm::vec3 direction;
  glm::vec3 color;
};

class World {
private:
  int nextID = 0;

public:
  std::unordered_map<int, VelocityComponent> velocities;
  std::unordered_map<int, TransformComponent> transforms;
  std::unordered_map<int, MeshComponent> meshes;
  std::unordered_map<int, LightComponent> lightSources;

  void addVelocityComponent(int entityID, VelocityComponent vel);
  void addTransformComponent(int entityID, TransformComponent trans);
  void addMeshComponent(int entityID, MeshComponent mesh);
  void addLightComponent(int entityID, LightComponent light);
  void destroyEntity(int entityID);
  int createEntity();
  std::optional<VelocityComponent> getVelocityComponent(int entityID);
  std::optional<TransformComponent> getTransformComponent(int entityID);
};
