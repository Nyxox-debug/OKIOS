#include "creature/mesh.hpp"
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

class World {
private:
  int nextID = 0;

public:
  std::unordered_map<int, VelocityComponent> velocities;
  std::unordered_map<int, TransformComponent> transforms;
  std::unordered_map<int, MeshComponent> meshes;

  void addVelocityComponent(int entityID, VelocityComponent vel);
  void addTransformComponent(int entityID, TransformComponent trans);
  void addMeshComponent(int entityID, MeshComponent mesh);
  int createEntity();
  std::optional<VelocityComponent> getVelocityComponent(int entityID);
  std::optional<TransformComponent> getTransformComponent(int entityID);
};
