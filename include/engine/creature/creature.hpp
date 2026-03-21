#include "glm/fwd.hpp"
#include "mesh.hpp"
#include "engine/transform.hpp"
#include <memory>

class Creature {
public:
  int ID;
  Creature(std::vector<std::unique_ptr<Mesh>> meshes);
  void Draw(Shader &shader) const;
  Transform transform;
  glm::vec3 velocity;

private:
  std::vector<std::unique_ptr<Mesh>> meshess;
};
