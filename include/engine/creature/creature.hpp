#include "mesh.hpp"
// #include "transform.h"
#include <memory>

struct Vec3 {
  float x, y, z;
};

class Creature {
public:
  int ID;
  Creature(std::vector<std::unique_ptr<Mesh>> meshes);
  void Draw(Shader &shader) const;
  Vec3 velocity;
  Vec3 position;


  // Transform transform;

private:
  std::vector<std::unique_ptr<Mesh>> meshess;
};
