#include "glm/fwd.hpp"
#include "../entity/entity.hpp"
#include "mesh.hpp"
// #include "engine/transform.hpp"
#include <memory>

class Creature : public Entity {
public:
  int ID;
  Creature(std::vector<std::unique_ptr<Mesh>> meshes);
  void Draw(Shader &shader) const;
  glm::vec3 velocity;

private:
  std::vector<std::unique_ptr<Mesh>> meshess;
};
