#include "mesh.hpp"
// #include "transform.h"
#include <memory>

class Creature {
public:
  Creature(std::vector<std::unique_ptr<Mesh>> meshes);
  void Draw(Shader &shader) const;

  // Transform transform;

private:
  std::vector<std::unique_ptr<Mesh>> meshess;
};
